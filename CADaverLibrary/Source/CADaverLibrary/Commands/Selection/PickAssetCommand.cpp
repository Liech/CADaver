#include "Commands/Selection/PickAssetCommand.h"

#include "CADacomb/Util/Ray.h"
#include "Database/Database.h"
#include "Picking/AssetPicker.h"
#include "Picking/PickResult.h"

PickAssetCommand::PickAssetCommand() {}

PickAssetCommand::~PickAssetCommand() {}

std::string PickAssetCommand::getType() const
{
    return "PickAsset";
}

int PickAssetCommand::getVersion() const
{
    return 1;
}

std::string PickAssetCommand::getDescription() const
{
    return "Command: PickAsset\n"
           "Version: 1\n"
           "Purpose: Performs CPU raycasting against project assets along a world-space ray.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"PickAsset\",\n"
           "  \"version\": 1,\n"
           "  \"origin\": [x, y, z],\n"
           "  \"direction\": [dx, dy, dz]\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"PickAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"hit\": <bool>,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"assetType\": <string>,\n"
           "  \"distance\": <number>,\n"
           "  \"hitPoint\": [x, y, z],\n"
           "  \"hasTriangle\": <bool>,\n"
           "  \"triangleIndex\": <integer>\n"
           "}";
}

void PickAssetCommand::setInput(const Polyglot::Node& node)
{
    nlohmann::json originJson = node["origin"].toJson();
    origin.x                  = originJson.at(0).get<double>();
    origin.y                  = originJson.at(1).get<double>();
    origin.z                  = originJson.at(2).get<double>();

    nlohmann::json dirJson = node["direction"].toJson();
    direction.x            = dirJson.at(0).get<double>();
    direction.y            = dirJson.at(1).get<double>();
    direction.z            = dirJson.at(2).get<double>();
}

nlohmann::json PickAssetCommand::execute(Database& database)
{
    AssetPicker   picker;
    CADacomb::Ray ray(origin, direction);
    PickResult    result = picker.pickRay(ray, database.getProject());

    if (!result.isHit())
    {
        return createResponse("Success", {
            { "hit", false }
        });
    }

    return createResponse("Success", {
        {           "hit", true },
        {     "assetGuid", result.getAssetGuid().toString() },
        {     "assetType", result.getAssetType() },
        {      "distance", result.getDistance() },
        {      "hitPoint", { result.getHitPoint().x, result.getHitPoint().y, result.getHitPoint().z } },
        {   "hasTriangle", result.hasTriangle() },
        { "triangleIndex", result.getTriangleIndex() }
    });
}

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("PickAssetCommand/RayHitAndMiss")
{
    auto tri      = std::make_unique<CADacomb::Triangulation>();
    tri->vertices = {
        glm::dvec3(-1.0, -1.0, 0.0),
        glm::dvec3( 1.0, -1.0, 0.0),
        glm::dvec3( 0.0,  1.0, 0.0)
    };
    tri->indices = { 0, 1, 2 };

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setTriangulation(std::move(tri));

    Database database;
    database.getProject().addAsset(asset);

    PickAssetCommand hitCmd;
    hitCmd.setInput(Polyglot::Node({
      {    "origin", { 0.0, 0.0, 5.0 } },
      { "direction", { 0.0, 0.0, -1.0 } }
    }));
    nlohmann::json hitRes = hitCmd.execute(database);
    CHECK(hitRes.at("status").get<std::string>() == "Success");
    CHECK(hitRes.at("hit").get<bool>() == true);
    CHECK(hitRes.at("assetGuid").get<std::string>() == asset->getGuid().toString());
    CHECK(hitRes.at("assetType").get<std::string>() == "Triangulation");
    CHECK(hitRes.at("hasTriangle").get<bool>() == true);
    CHECK(hitRes.at("triangleIndex").get<size_t>() == 0);

    PickAssetCommand missCmd;
    missCmd.setInput(Polyglot::Node({
      {    "origin", { 10.0, 10.0, 5.0 } },
      { "direction", {  0.0,  0.0, -1.0 } }
    }));
    nlohmann::json missRes = missCmd.execute(database);
    CHECK(missRes.at("status").get<std::string>() == "Success");
    CHECK(missRes.at("hit").get<bool>() == false);
}

TEST_CASE("PickAssetCommand/OffsetTriangulationBounds")
{
    auto tri      = std::make_unique<CADacomb::Triangulation>();
    tri->vertices = {
        glm::dvec3(146.0, 63.0, 10.0),
        glm::dvec3(201.0, 63.0, 10.0),
        glm::dvec3(173.0, 129.0, 10.0)
    };
    tri->indices = { 0, 1, 2 };

    auto asset = std::make_shared<TriangulationAsset>();
    asset->setTriangulation(std::move(tri));

    Database database;
    database.getProject().addAsset(asset);

    PickAssetCommand hitCmd;
    hitCmd.setInput(Polyglot::Node({
      {    "origin", { 173.0, 80.0, 100.0 } },
      { "direction", {   0.0,  0.0,  -1.0 } }
    }));
    nlohmann::json hitRes = hitCmd.execute(database);
    CHECK(hitRes.at("status").get<std::string>() == "Success");
    CHECK(hitRes.at("hit").get<bool>() == true);
    CHECK(hitRes.at("assetGuid").get<std::string>() == asset->getGuid().toString());
}
#endif
