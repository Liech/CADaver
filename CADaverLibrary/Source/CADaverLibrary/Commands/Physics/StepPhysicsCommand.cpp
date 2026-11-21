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
#endif
