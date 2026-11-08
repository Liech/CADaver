#include "Commands/Geometry/GetAssetAABBCommand.h"

#include "CADacomb/CAD/Interface/Shape.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

GetAssetAABBCommand::GetAssetAABBCommand()
{
}

GetAssetAABBCommand::~GetAssetAABBCommand()
{
}

std::string GetAssetAABBCommand::getType() const
{
    return "GetAssetAABB";
}

int GetAssetAABBCommand::getVersion() const
{
    return 1;
}

std::string GetAssetAABBCommand::getDescription() const
{
    return "Command: GetAssetAABB\n"
           "Version: 1\n"
           "Purpose: Queries the axis-aligned bounding box (min and max corners) of any geometry asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetAABB\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string> (optional),\n"
           "  \"assetName\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetAABB\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\",\n"
           "  \"assetGuid\": <string>,\n"
           "  \"min\": [<double>, <double>, <double>],\n"
           "  \"max\": [<double>, <double>, <double>]\n"
           "}";
}

void GetAssetAABBCommand::setInput(const Polyglot::Node& node)
{
    if (node.contains("assetGuid"))
    {
        assetGuid = node["assetGuid"].asString();
    }
    if (node.contains("assetName"))
    {
        assetName = node["assetName"].asString();
    }
}

nlohmann::json GetAssetAABBCommand::execute(Database& database)
{
    std::shared_ptr<Asset> foundAsset = nullptr;

    if (!assetGuid.toString().empty())
    {
        foundAsset = database.getProject().getAsset(assetGuid);
    }

    if (!foundAsset && !assetName.empty())
    {
        const auto& assets = database.getProject().getAssets();
        for (const auto& asset : assets)
        {
            if (asset && asset->getName() == assetName)
            {
                foundAsset = asset;
                break;
            }
        }
    }

    if (!foundAsset)
    {
        return createResponse("AssetNotFound", {
            { "assetGuid", assetGuid.toString() },
            {       "min",          { 0, 0, 0 } },
            {       "max",          { 0, 0, 0 } }
        });
    }

    glm::dvec3 minCorner(0.0);
    glm::dvec3 maxCorner(0.0);

    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(foundAsset);
    if (voxelAsset)
    {
        const auto& aabb = voxelAsset->getAABB();
        minCorner        = aabb.getMin();
        maxCorner        = aabb.getMax();
    }
    else
    {
        auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(foundAsset);
        if (triAsset)
        {
            auto aabb = triAsset->getTriangulation().getAABB();
            minCorner = aabb.first;
            maxCorner = aabb.second;
        }
        else
        {
            auto cadAsset = std::dynamic_pointer_cast<CadAsset>(foundAsset);
            if (cadAsset)
            {
                try
                {
                    auto aabb = cadAsset->getCadShape().getAABB();
                    minCorner = aabb.getMin();
                    maxCorner = aabb.getMax();
                }
                catch (...)
                {
                    auto aabb = cadAsset->getTriangulation().getAABB();
                    minCorner = aabb.first;
                    maxCorner = aabb.second;
                }
            }
        }
    }

    return createResponse("Success", {
        { "assetGuid", foundAsset->getGuid().toString() },
        {       "min",    { minCorner.x, minCorner.y, minCorner.z } },
        {       "max",    { maxCorner.x, maxCorner.y, maxCorner.z } }
    });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetAABBCommand/NotFound")
{
    Database            database;
    GetAssetAABBCommand cmd;
    cmd.setInput({
        {      "type",  "GetAssetAABB" },
        {   "version",               1 },
        { "assetName",   "NonExistent" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "AssetNotFound");
}

TEST_CASE("GetAssetAABBCommand/VoxelAABB")
{
    Database database;
    auto     voxelAsset = std::make_shared<VoxelAsset>();
    voxelAsset->setName("BoxModel");
    voxelAsset->setAABB(CADacomb::AABB3(glm::vec3(-5, -5, -5), glm::vec3(5, 5, 5)));
    database.getProject().addAsset(voxelAsset);

    GetAssetAABBCommand cmd;
    cmd.setInput({
        {      "type", "GetAssetAABB" },
        {   "version",              1 },
        { "assetName",     "BoxModel" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("min")[0] == -5.0);
    CHECK(result.at("max")[0] == 5.0);
}
#endif
