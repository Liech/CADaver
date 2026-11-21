#include "Commands/Physics/StepPhysicsCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"
#include "Database/Project.h"

StepPhysicsCommand::StepPhysicsCommand() {}

StepPhysicsCommand::~StepPhysicsCommand() {}

std::string StepPhysicsCommand::getType() const
{
    return "StepPhysics";
}

int StepPhysicsCommand::getVersion() const
{
    return 1;
}

std::string StepPhysicsCommand::getDescription() const
{
    return "Command: StepPhysics\n"
           "Version: 1\n"
           "Purpose: Advances the rigid body physics simulation of the project by a given time delta.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"StepPhysics\",\n"
           "  \"version\": 1,\n"
           "  \"deltaTime\": <double> (optional, default: 0.016666666666666666),\n"
           "  \"mergeGuid\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"StepPhysics\",\n"
           "  \"version\": 1,\n"
           "  \"deltaTime\": <double>\n"
           "}";
}

void StepPhysicsCommand::setInput(const Polyglot::Node& node)
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

nlohmann::json StepPhysicsCommand::execute(Database& database)
{
    if (deltaTime <= 0.0)
    {
        return createErrorResponse("InvalidDeltaTime", "deltaTime must be positive");
    }

    previousTransformations.clear();
    currentTransformations.clear();

    for (const auto& asset : database.getProject().getAssets())
    {
        previousTransformations[asset->getGuid()] = asset->getTransformation();
    }

    database.getPhysicsSimulation().step(database.getProject(), deltaTime, getMergeGuid());

    for (const auto& asset : database.getProject().getAssets())
    {
        currentTransformations[asset->getGuid()] = asset->getTransformation();
    }

    return createResponse({
      { "deltaTime", deltaTime }
    });
}

bool StepPhysicsCommand::isUndoable() const
{
    return true;
}

void StepPhysicsCommand::undo(Database& database)
{
    database.getPhysicsSimulation().reset();
    for (const auto& pair : previousTransformations)
    {
        auto asset = database.getProject().getAsset(pair.first);
        if (asset)
        {
            asset->setTransformation(pair.second);
        }
    }
}

void StepPhysicsCommand::redo(Database& database)
{
    database.getPhysicsSimulation().reset();
    for (const auto& pair : currentTransformations)
    {
        auto asset = database.getProject().getAsset(pair.first);
        if (asset)
        {
            asset->setTransformation(pair.second);
        }
    }
}

size_t StepPhysicsCommand::getUndoSize() const
{
    return sizeof(StepPhysicsCommand) + (previousTransformations.size() + currentTransformations.size()) * (sizeof(CADacomb::GUID) + sizeof(glm::dmat4));
}

bool StepPhysicsCommand::isMergeable() const
{
    return true;
}

bool StepPhysicsCommand::merge(const Command& other)
{
    const auto* otherStep = dynamic_cast<const StepPhysicsCommand*>(&other);
    if (!otherStep)
    {
        return false;
    }

    deltaTime += otherStep->deltaTime;
    for (const auto& pair : otherStep->currentTransformations)
    {
        currentTransformations[pair.first] = pair.second;
    }
    return true;
}

double StepPhysicsCommand::getDeltaTime() const
{
    return deltaTime;
}

#ifdef ISTESTPROJECT
#include "CADacomb/CAD/Sm/KernelSm.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/RigidBodyComponent.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Utility/MatrixJson.h"

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("StepPhysicsCommand/MergedInteractiveRunMatchesSingleHeadlessCommand")
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
          {      "type",       "StepPhysics" },
          {   "version",                   1 },
          { "deltaTime",                  dt },
          { "mergeGuid", playGuid.toString() }
        });
    }

    REQUIRE(interactiveUndo.getUndoHistory().size() == 1);

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
      {      "type",                   "StepPhysics" },
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
    Polyglot::Node undoneRes = interactiveDb.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",              incCubeGuid }
    });
    CHECK(MatrixJson::fromJson(undoneRes["transformation"].toJson()) == glm::translate(glm::dmat4(1.0), glm::dvec3(0.0, 3.0, 0.0)));

    interactiveUndo.redo();
    Polyglot::Node redoneRes = interactiveDb.execute({
      {      "type", "GetTransformationAsset" },
      {   "version",                        1 },
      { "assetGuid",              incCubeGuid }
    });
    CHECK(MatrixJson::fromJson(redoneRes["transformation"].toJson()) == incFinalTransform);
}

TEST_CASE("StepPhysicsCommand/CadAssetRigidBodySimulation")
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

    StepPhysicsCommand stepCmd;
    stepCmd.setInput({
      {      "type", "StepPhysics" },
      {   "version",             1 },
      { "deltaTime",           0.5 }
    });
    nlohmann::json res = stepCmd.execute(db);
    CHECK(res.at("type").get<std::string>() == "StepPhysics");
    CHECK(boxAsset->getTransformation()[3].y < 3.0);
    CHECK(floorAsset->getTransformation()[3].y == -1.0);
}

TEST_CASE("StepPhysicsCommand/VoxelAssetSVORigidBodyAndDynamicChange")
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

    StepPhysicsCommand step1;
    step1.setInput({
      {      "type",          "StepPhysics" },
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

    StepPhysicsCommand step2;
    step2.setInput({
      {      "type",          "StepPhysics" },
      {   "version",                      1 },
      { "deltaTime",                    1.0 },
      { "mergeGuid", sessionGuid.toString() }
    });
    step2.execute(db);

    CHECK(blockAsset->getTransformation()[3].y < -2.0);
}

TEST_CASE("StepPhysicsCommand/SteppedMultiLeafSVOTerrainCollision")
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

    StepPhysicsCommand stepCmd;
    stepCmd.setInput({
      {      "type", "StepPhysics" },
      {   "version",             1 },
      { "deltaTime",           1.0 }
    });
    nlohmann::json res = stepCmd.execute(db);
    CHECK(res.at("type").get<std::string>() == "StepPhysics");
    CHECK(fallingAsset->getTransformation()[3].y < 2.0);
    CHECK(fallingAsset->getTransformation()[3].y > -6.0);
}
#endif
