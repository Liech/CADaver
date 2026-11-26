#include "Commands/Geometry/SimplifyMeshCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Conversion/AssetConverter.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Database.h"

SimplifyMeshCommand::SimplifyMeshCommand() {}

SimplifyMeshCommand::~SimplifyMeshCommand() {}

std::string SimplifyMeshCommand::getType() const
{
    return "SimplifyMesh";
}

int SimplifyMeshCommand::getVersion() const
{
    return 1;
}

std::string SimplifyMeshCommand::getDescription() const
{
    return "Command: SimplifyMesh\n"
           "Version: 1\n"
           "Purpose: Simplifies a triangulation mesh asset by target percentage or target triangle count.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SimplifyMesh\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"mode\": \"Percent\" | \"TriangleCount\",\n"
           "  \"targetPercent\": <double>,\n"
           "  \"targetTriangleCount\": <size_t>,\n"
           "  \"deleteOrigin\": <bool>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SimplifyMesh\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"triangleCount\": <size_t>\n"
           "}";
}

std::vector<CADacomb::GUID> SimplifyMeshCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SimplifyMeshCommand::setInput(const Polyglot::Node& node)
{
    assetGuid           = node["assetGuid"].asString();
    mode                = node["mode"].asString();
    targetPercent       = node["targetPercent"].asDouble();
    targetTriangleCount = static_cast<size_t>(node["targetTriangleCount"].asInt());
    deleteOrigin        = node["deleteOrigin"].asBool();
}

nlohmann::json SimplifyMeshCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(1, "Simplifying mesh");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(baseAsset);
    if (!triAsset || bar.isCanceled())
    {
        return createErrorResponse("NotTriangulationAsset", "Asset is not a TriangulationAsset");
    }

    std::shared_ptr<TriangulationAsset> resultAsset;
    if (mode == "Percent")
    {
        resultAsset = AssetConverter::simplifyMeshByPercent(*triAsset, targetPercent);
    }
    else if (mode == "TriangleCount")
    {
        resultAsset = AssetConverter::simplifyMeshByTriangleCount(*triAsset, targetTriangleCount);
    }
    else
    {
        return createErrorResponse("InvalidMode", "Invalid simplification mode: " + mode);
    }

    if (!resultAsset || bar.isCanceled())
    {
        return createErrorResponse("SimplificationFailed", "Failed to simplify mesh asset");
    }

    bar.addProgress(1);

    database.getProject().addAsset(resultAsset);
    createdAsset = resultAsset;

    if (deleteOrigin)
    {
        originAsset = baseAsset;
        database.getProject().removeAsset(assetGuid);
    }

    size_t triangleCount = resultAsset->getTriangulation().indices.size() / 3;
    return createResponse({
      {     "assetGuid", resultAsset->getGuid().toString() },
      { "triangleCount",                     triangleCount }
    });
}

bool SimplifyMeshCommand::isUndoable() const
{
    return createdAsset != nullptr;
}

void SimplifyMeshCommand::undo(Database& database)
{
    if (createdAsset)
    {
        database.getProject().removeAsset(createdAsset->getGuid());
    }
    if (deleteOrigin && originAsset)
    {
        database.getProject().addAsset(originAsset);
    }
}

void SimplifyMeshCommand::redo(Database& database)
{
    if (deleteOrigin && originAsset)
    {
        database.getProject().removeAsset(originAsset->getGuid());
    }
    if (createdAsset)
    {
        database.getProject().addAsset(createdAsset);
    }
}

size_t SimplifyMeshCommand::getUndoSize() const
{
    size_t size = sizeof(SimplifyMeshCommand);
    if (originAsset)
    {
        size += originAsset->getSizeInBytes();
    }
    if (createdAsset)
    {
        size += createdAsset->getSizeInBytes();
    }
    return size;
}

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/BaseShapeGenerator.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SimplifyMeshCommand/SimplifyByPercentAndCount")
{
    Database database;
    auto     triAsset = std::make_shared<TriangulationAsset>();
    triAsset->setName("Sphere.stl");
    auto   sphere   = std::make_unique<CADacomb::Triangulation>(CADacomb::BaseShapeGenerator::sphere(20, 10, 1.0f));
    size_t origTris = sphere->indices.size() / 3;
    triAsset->setTriangulation(std::move(sphere));
    database.getProject().addAsset(triAsset);

    SimplifyMeshCommand pctCmd;
    pctCmd.setInput(Polyglot::Node({
      {           "assetGuid", triAsset->getGuid().toString() },
      {                "mode",                      "Percent" },
      {       "targetPercent",                           50.0 },
      { "targetTriangleCount",                              0 },
      {        "deleteOrigin",                          false }
    }));

    nlohmann::json pctRes = pctCmd.execute(database);
    REQUIRE(pctRes.at("type").get<std::string>() == "SimplifyMesh");
    CHECK(pctRes.at("triangleCount").get<size_t>() <= origTris / 2 + 2);

    SimplifyMeshCommand countCmd;
    countCmd.setInput(Polyglot::Node({
      {           "assetGuid", triAsset->getGuid().toString() },
      {                "mode",                "TriangleCount" },
      {       "targetPercent",                            0.0 },
      { "targetTriangleCount",                             60 },
      {        "deleteOrigin",                          false }
    }));

    nlohmann::json countRes = countCmd.execute(database);
    REQUIRE(countRes.at("type").get<std::string>() == "SimplifyMesh");
    CHECK(countRes.at("triangleCount").get<size_t>() <= 60);

    CADacomb::GUID createdGuid(countRes.at("assetGuid").get<std::string>());
    countCmd.undo(database);
    CHECK(database.getProject().getAsset(createdGuid) == nullptr);
    countCmd.redo(database);
    CHECK(database.getProject().getAsset(createdGuid) != nullptr);
}
#endif
