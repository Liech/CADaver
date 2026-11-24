#include "Commands/Time/StepTimeCommand.h"

#include "Database/Asset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/SoftBodyComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"

StepTimeCommand::StepTimeCommand() {}

StepTimeCommand::~StepTimeCommand() {}

std::string StepTimeCommand::getType() const
{
    return "StepTime";
}

int StepTimeCommand::getVersion() const
{
    return 1;
}

std::string StepTimeCommand::getDescription() const
{
    return "Command: StepTime\n"
           "Version: 1\n"
           "Purpose: Advances the project time by a given time delta, stepping all time-dependent project systems.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"StepTime\",\n"
           "  \"version\": 1,\n"
           "  \"deltaTime\": <double> (optional, default: 0.016666666666666666),\n"
           "  \"mergeGuid\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"StepTime\",\n"
           "  \"version\": 1,\n"
           "  \"deltaTime\": <double>,\n"
           "  \"currentTime\": <double>\n"
           "}";
}

void StepTimeCommand::setInput(const Polyglot::Node& node)
{
    deltaTime = 1.0 / 60.0;
    if (node.contains("deltaTime") && !node["deltaTime"].isNull())
    {
        deltaTime = node["deltaTime"].asDouble();
    }
    if (node.contains("mergeGuid") && !node["mergeGuid"].isNull())
    {
        setMergeGuid(CADacomb::GUID(node["mergeGuid"].asString()));
    }
}

nlohmann::json StepTimeCommand::execute(Database& database)
{
    if (deltaTime <= 0.0)
    {
        return createErrorResponse("InvalidDeltaTime", "deltaTime must be positive");
    }

    previousTime = database.getCurrentTime();
    currentTime  = previousTime + deltaTime;

    previousTransformations.clear();
    currentTransformations.clear();
    previousVertices.clear();
    currentVertices.clear();

    for (const auto& asset : database.getProject().getAssets())
    {
        previousTransformations[asset->getGuid()] = asset->getTransformation();
        if (!asset->getAttachments<SoftBodyComponent>().empty())
        {
            if (auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset))
            {
                previousVertices[asset->getGuid()] = triAsset->getTriangulation().vertices;
            }
        }
    }

    database.setCurrentTime(currentTime);
    database.getPhysicsSimulation().step(database.getProject(), deltaTime, getMergeGuid());

    for (const auto& asset : database.getProject().getAssets())
    {
        currentTransformations[asset->getGuid()] = asset->getTransformation();
        if (!asset->getAttachments<SoftBodyComponent>().empty())
        {
            if (auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset))
            {
                currentVertices[asset->getGuid()] = triAsset->getTriangulation().vertices;
            }
        }
    }

    return createResponse({
      {   "deltaTime",   deltaTime },
      { "currentTime", currentTime }
    });
}

bool StepTimeCommand::isUndoable() const
{
    return true;
}

void StepTimeCommand::undo(Database& database)
{
    database.setCurrentTime(previousTime);
    database.getPhysicsSimulation().reset();
    for (const auto& pair : previousTransformations)
    {
        auto asset = database.getProject().getAsset(pair.first);
        if (asset)
        {
            asset->setTransformation(pair.second);
        }
    }
    for (const auto& pair : previousVertices)
    {
        auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(database.getProject().getAsset(pair.first));
        if (triAsset)
        {
            triAsset->getTriangulation().vertices = pair.second;
            triAsset->setMeshColor(triAsset->getMeshColor());
        }
    }
}

void StepTimeCommand::redo(Database& database)
{
    database.setCurrentTime(currentTime);
    database.getPhysicsSimulation().reset();
    for (const auto& pair : currentTransformations)
    {
        auto asset = database.getProject().getAsset(pair.first);
        if (asset)
        {
            asset->setTransformation(pair.second);
        }
    }
    for (const auto& pair : currentVertices)
    {
        auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(database.getProject().getAsset(pair.first));
        if (triAsset)
        {
            triAsset->getTriangulation().vertices = pair.second;
            triAsset->setMeshColor(triAsset->getMeshColor());
        }
    }
}

size_t StepTimeCommand::getUndoSize() const
{
    size_t totalBytes = sizeof(StepTimeCommand) + (previousTransformations.size() + currentTransformations.size()) * (sizeof(CADacomb::GUID) + sizeof(glm::dmat4));
    for (const auto& pair : previousVertices)
    {
        totalBytes += sizeof(CADacomb::GUID) + pair.second.size() * sizeof(glm::dvec3);
    }
    for (const auto& pair : currentVertices)
    {
        totalBytes += sizeof(CADacomb::GUID) + pair.second.size() * sizeof(glm::dvec3);
    }
    return totalBytes;
}

bool StepTimeCommand::isMergeable() const
{
    return true;
}

bool StepTimeCommand::merge(const Command& other)
{
    const auto* otherStep = dynamic_cast<const StepTimeCommand*>(&other);
    if (!otherStep)
    {
        return false;
    }

    deltaTime += otherStep->deltaTime;
    currentTime = otherStep->currentTime;
    for (const auto& pair : otherStep->currentTransformations)
    {
        currentTransformations[pair.first] = pair.second;
    }
    for (const auto& pair : otherStep->currentVertices)
    {
        currentVertices[pair.first] = pair.second;
    }
    return true;
}

double StepTimeCommand::getDeltaTime() const
{
    return deltaTime;
}

#ifdef ISTESTPROJECT
#include "CADacomb/CAD/Sm/KernelSm.h"
#include "CADacomb/Triangle/BaseShapeGenerator.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/RigidBodyComponent.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Utility/MatrixJson.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("StepTimeCommand/MergedInteractiveRunMatchesSingleHeadlessCommand")
{
    auto setupScene = [](GatedDatabase& db, std::string& floorGuidOut, std::string& cubeGuidOut)
    {
        Polyglot::Node floorRes = db.execute({
          {    "type", "GenerateBaseShape" },
          { "version",                   1 },
          {   "shape",               "box" },
          {   "width",                20.0 },
          {   "depth",                20.0 },
          {  "height",                 1.0 }
        });
        floorGuidOut            = floorRes["assetGuid"].asString();

        glm::dmat4 floorTf = glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, -1.0, 0.0));
        db.execute({
          {           "type",            "TransformAsset" },
          {        "version",                           1 },
          {      "assetGuid",                floorGuidOut },
          { "transformation", MatrixJson::toJson(floorTf) }
        });
        db.execute({
          {          "type",                             "AddComponent" },
          {       "version",                                          1 },
          {     "assetGuid",                               floorGuidOut },
          { "componentType",                                "RigidBody" },
          {          "name",                                "RigidBody" },
          {          "data", { { "Version", 1 }, { "isStatic", true } } }
        });

        Polyglot::Node cubeRes = db.execute({
          {    "type", "GenerateBaseShape" },
          { "version",                   1 },
          {   "shape",              "cube" }
        });
        cubeGuidOut            = cubeRes["assetGuid"].asString();

        glm::dmat4 cubeTf = glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 3.0, 0.0));
        db.execute({
          {           "type",           "TransformAsset" },
          {        "version",                          1 },
          {      "assetGuid",                cubeGuidOut },
          { "transformation", MatrixJson::toJson(cubeTf) }
        });
        db.execute({
          {          "type",                              "AddComponent" },
          {       "version",                                           1 },
          {     "assetGuid",                                 cubeGuidOut },
          { "componentType",                                 "RigidBody" },
          {          "name",                                 "RigidBody" },
          {          "data", { { "Version", 1 }, { "isStatic", false } } }
        });
    };

    GatedDatabase interactiveDb;
    UndoStack     interactiveUndo(interactiveDb);
    std::string   incFloorGuid;
    std::string   incCubeGuid;
    setupScene(interactiveDb, incFloorGuid, incCubeGuid);
    interactiveUndo.clear();

    CADacomb::GUID playGuid = CADacomb::GUID::random();
    const int      steps    = 30;
    const double   dt       = 1.0 / 60.0;
    for (int i = 0; i < steps; i++)
    {
        interactiveDb.execute({
          {      "type",          "StepTime" },
          {   "version",                   1 },
          { "deltaTime",                  dt },
          { "mergeGuid", playGuid.toString() }
        });
    }

    REQUIRE(interactiveUndo.getUndoHistory().size() == 1);

    Polyglot::Node timeAfterSteps = interactiveDb.execute({
      {    "type", "GetCurrentTime" },
      { "version",                1 }
    });
    CHECK(timeAfterSteps["currentTime"].asDouble() == Catch::Approx(static_cast<double>(steps) * dt));

    Polyglot::Node incTransformRes   = interactiveDb.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",              incCubeGuid }
    });
    glm::dmat4     incFinalTransform = MatrixJson::fromJson(incTransformRes["transformation"].toJson());
    CHECK(incFinalTransform[3].y < 3.0);

    GatedDatabase headlessDb;
    std::string   headlessFloorGuid;
    std::string   headlessCubeGuid;
    setupScene(headlessDb, headlessFloorGuid, headlessCubeGuid);

    headlessDb.execute({
      {      "type",                      "StepTime" },
      {   "version",                               1 },
      { "deltaTime", static_cast<double>(steps) * dt }
    });

    Polyglot::Node headlessTransformRes   = headlessDb.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",         headlessCubeGuid }
    });
    glm::dmat4     headlessFinalTransform = MatrixJson::fromJson(headlessTransformRes["transformation"].toJson());

    CHECK(incFinalTransform == headlessFinalTransform);

    interactiveUndo.undo();
    Polyglot::Node timeAfterUndo = interactiveDb.execute({
      {    "type", "GetCurrentTime" },
      { "version",                1 }
    });
    CHECK(timeAfterUndo["currentTime"].asDouble() == Catch::Approx(0.0));

    Polyglot::Node undoneRes = interactiveDb.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",              incCubeGuid }
    });
    CHECK(MatrixJson::fromJson(undoneRes["transformation"].toJson()) == glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 3.0, 0.0)));

    interactiveUndo.redo();
    Polyglot::Node timeAfterRedo = interactiveDb.execute({
      {    "type", "GetCurrentTime" },
      { "version",                1 }
    });
    CHECK(timeAfterRedo["currentTime"].asDouble() == Catch::Approx(static_cast<double>(steps) * dt));

    Polyglot::Node redoneRes = interactiveDb.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",              incCubeGuid }
    });
    CHECK(MatrixJson::fromJson(redoneRes["transformation"].toJson()) == incFinalTransform);
}

TEST_CASE("StepTimeCommand/CadAssetRigidBodySimulation")
{
    Database db;

    CADacomb::CAD::KernelSm kernel;
    auto                    floorSolid = kernel.createBox(glm::dvec3(-10.0, -0.5, -10.0), glm::dvec3(20.0, 1.0, 20.0));
    auto                    floorAsset = std::make_shared<CadAsset>();
    floorAsset->setName("CadFloor");
    floorAsset->setCadShape(std::shared_ptr<CADacomb::CAD::Shape>(std::move(floorSolid)));
    floorAsset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, -1.0, 0.0)));
    floorAsset->addComponent(std::make_shared<RigidBodyComponent>(true));
    db.getProject().addAsset(floorAsset);

    auto boxSolid = kernel.createBox(glm::dvec3(-0.5, -0.5, -0.5), glm::dvec3(1.0, 1.0, 1.0));
    auto boxAsset = std::make_shared<CadAsset>();
    boxAsset->setName("CadBox");
    boxAsset->setCadShape(std::shared_ptr<CADacomb::CAD::Shape>(std::move(boxSolid)));
    boxAsset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 3.0, 0.0)));
    boxAsset->addComponent(std::make_shared<RigidBodyComponent>(false));
    db.getProject().addAsset(boxAsset);

    StepTimeCommand stepCmd;
    stepCmd.setInput({
      {      "type", "StepTime" },
      {   "version",          1 },
      { "deltaTime",        0.5 }
    });
    nlohmann::json res = stepCmd.execute(db);
    CHECK(res.at("type").get<std::string>() == "StepTime");
    CHECK(res.at("currentTime").get<double>() == Catch::Approx(0.5));
    CHECK(boxAsset->getTransformation()[3].y < 3.0);
    CHECK(floorAsset->getTransformation()[3].y == -1.0);
}

TEST_CASE("StepTimeCommand/VoxelAssetSVORigidBodyAndDynamicChange")
{
    Database db;

    auto floorSvo = std::make_unique<CADacomb::SVO<bool>>(16, false);
    for (int x = 0; x < 16; x++)
    {
        for (int z = 0; z < 16; z++)
        {
            floorSvo->setVoxel(glm::ivec3(x, 7, z), true);
        }
    }
    auto floorAsset = std::make_shared<VoxelAsset>();
    floorAsset->setName("VoxelFloor");
    floorAsset->setSVO(std::move(floorSvo));
    floorAsset->setAABB(CADacomb::AABB3(glm::dvec3(-8.0, -8.0, -8.0), glm::dvec3(8.0, 8.0, 8.0)));
    floorAsset->addComponent(std::make_shared<RigidBodyComponent>(true));
    db.getProject().addAsset(floorAsset);

    auto blockSvo   = std::make_unique<CADacomb::SVO<bool>>(4, true);
    auto blockAsset = std::make_shared<VoxelAsset>();
    blockAsset->setName("VoxelBlock");
    blockAsset->setSVO(std::move(blockSvo));
    blockAsset->setAABB(CADacomb::AABB3(glm::dvec3(-0.5, -0.5, -0.5), glm::dvec3(0.5, 0.5, 0.5)));
    blockAsset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 2.0, 0.0)));
    blockAsset->addComponent(std::make_shared<RigidBodyComponent>(false));
    db.getProject().addAsset(blockAsset);

    CADacomb::GUID sessionGuid = CADacomb::GUID::random();

    StepTimeCommand step1;
    step1.setInput({
      {      "type",             "StepTime" },
      {   "version",                      1 },
      { "deltaTime",                    1.0 },
      { "mergeGuid", sessionGuid.toString() }
    });
    step1.execute(db);

    double restingY = blockAsset->getTransformation()[3].y;
    CHECK(restingY < 1.5);
    CHECK(restingY > -0.2);

    for (int x = 0; x < 16; x++)
    {
        for (int z = 0; z < 16; z++)
        {
            floorAsset->getSVO().setVoxel(glm::ivec3(x, 7, z), false);
        }
    }
    floorAsset->setAABB(floorAsset->getAABB());

    StepTimeCommand step2;
    step2.setInput({
      {      "type",             "StepTime" },
      {   "version",                      1 },
      { "deltaTime",                    1.0 },
      { "mergeGuid", sessionGuid.toString() }
    });
    step2.execute(db);

    CHECK(blockAsset->getTransformation()[3].y < -2.0);
}

TEST_CASE("StepTimeCommand/SteppedMultiLeafSVOTerrainCollision")
{
    Database db;

    auto terrainSvo = std::make_unique<CADacomb::SVO<bool>>(32, false);
    for (int x = 0; x < 32; x++)
    {
        for (int z = 0; z < 32; z++)
        {
            int maxH = 8 + ((x + z) % 5);
            for (int y = 0; y <= maxH; y++)
            {
                terrainSvo->setVoxel(glm::ivec3(x, y, z), true);
            }
        }
    }
    auto terrainAsset = std::make_shared<VoxelAsset>();
    terrainAsset->setName("SteppedTerrain");
    terrainAsset->setSVO(std::move(terrainSvo));
    terrainAsset->setAABB(CADacomb::AABB3(glm::dvec3(-16.0, -16.0, -16.0), glm::dvec3(16.0, 16.0, 16.0)));
    terrainAsset->addComponent(std::make_shared<RigidBodyComponent>(true));
    db.getProject().addAsset(terrainAsset);

    auto fallingSvo = std::make_unique<CADacomb::SVO<bool>>(8, false);
    for (int x = 1; x < 7; x++)
    {
        for (int y = 1; y < 7; y++)
        {
            for (int z = 1; z < 7; z++)
            {
                if ((x + y + z) % 2 == 0)
                {
                    fallingSvo->setVoxel(glm::ivec3(x, y, z), true);
                }
            }
        }
    }
    auto fallingAsset = std::make_shared<VoxelAsset>();
    fallingAsset->setName("MultiLeafFalling");
    fallingAsset->setSVO(std::move(fallingSvo));
    fallingAsset->setAABB(CADacomb::AABB3(glm::dvec3(-2.0, -2.0, -2.0), glm::dvec3(2.0, 2.0, 2.0)));
    fallingAsset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 2.0, 0.0)));
    fallingAsset->addComponent(std::make_shared<RigidBodyComponent>(false));
    db.getProject().addAsset(fallingAsset);

    StepTimeCommand stepCmd;
    stepCmd.setInput({
      {      "type", "StepTime" },
      {   "version",          1 },
      { "deltaTime",        1.0 }
    });
    nlohmann::json res = stepCmd.execute(db);
    CHECK(res.at("type").get<std::string>() == "StepTime");
    CHECK(fallingAsset->getTransformation()[3].y < 2.0);
    CHECK(fallingAsset->getTransformation()[3].y > -6.0);
}

TEST_CASE("StepTimeCommand/SoftBodyDeformationUndoRedoAndMerge")
{
    Database db;

    auto floorAsset = std::make_shared<TriangulationAsset>();
    floorAsset->setName("Floor");
    floorAsset->setTriangulation(std::make_unique<CADacomb::Triangulation>(CADacomb::BaseShapeGenerator::box(20.0f, 1.0f, 20.0f)));
    floorAsset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, -1.0, 0.0)));
    floorAsset->addComponent(std::make_shared<RigidBodyComponent>(true));
    db.getProject().addAsset(floorAsset);

    auto       softAsset = std::make_shared<TriangulationAsset>();
    glm::dmat4 initialTf = glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 1.8, 0.0));
    softAsset->setName("SoftSphere");
    softAsset->setTriangulation(std::make_unique<CADacomb::Triangulation>(CADacomb::BaseShapeGenerator::sphere(1.0f, 12, 12)));
    softAsset->setTransformation(initialTf);
    softAsset->addComponent(std::make_shared<SoftBodyComponent>(CADacomb::Physics::SoftBodyMode::Volumetric, 1.0, 0.0, 0.0001, 0.001, 0.0002, 4, 0.25, 0.4, 0.0, 12));
    db.getProject().addAsset(softAsset);

    std::vector<glm::dvec3> initialVertices = softAsset->getTriangulation().vertices;
    CADacomb::GUID          playGuid        = CADacomb::GUID::random();

    StepTimeCommand mergedStep;
    for (int i = 0; i < 30; i++)
    {
        StepTimeCommand stepCmd;
        stepCmd.setInput({
          {      "type",          "StepTime" },
          {   "version",                   1 },
          { "deltaTime",          1.0 / 60.0 },
          { "mergeGuid", playGuid.toString() }
        });
        stepCmd.execute(db);
        if (i == 0)
        {
            mergedStep = stepCmd;
        }
        else
        {
            REQUIRE(mergedStep.merge(stepCmd));
        }
    }

    glm::dmat4              deformedTf       = softAsset->getTransformation();
    std::vector<glm::dvec3> deformedVertices = softAsset->getTriangulation().vertices;
    CHECK(deformedTf[3].y < 1.8);
    CHECK(deformedVertices != initialVertices);
    CHECK(db.getCurrentTime() == Catch::Approx(0.5));

    mergedStep.undo(db);
    CHECK(softAsset->getTransformation() == initialTf);
    CHECK(softAsset->getTriangulation().vertices == initialVertices);
    CHECK(db.getCurrentTime() == Catch::Approx(0.0));

    mergedStep.redo(db);
    CHECK(softAsset->getTransformation() == deformedTf);
    CHECK(softAsset->getTriangulation().vertices == deformedVertices);
    CHECK(db.getCurrentTime() == Catch::Approx(0.5));
}
#endif
