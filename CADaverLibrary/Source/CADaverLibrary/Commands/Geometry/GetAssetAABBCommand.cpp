#include "Commands/Geometry/GetAssetAABBCommand.h"

#include "CADacomb/Util/AABB3.h"
#include "Database/Database.h"
#include "Picking/AssetRayPicker/AssetRayPicker.h"
#include "Picking/AssetRayPicker/AssetRayPickerFactory.h"

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
        return createErrorResponse("AssetNotFound", "Asset not found");
    }

    CADacomb::AABB3       worldBounds;
    AssetRayPickerFactory pickerFactory;
    auto                  picker = pickerFactory.create(foundAsset->getType());
    if (picker)
    {
        CADacomb::AABB3 localBounds = picker->getAABB(*foundAsset);
        if (localBounds.getMin().x <= localBounds.getMax().x)
        {
            worldBounds = localBounds.transformed(foundAsset->getTransformation());
        }
    }

    return createResponse({
        { "assetGuid", foundAsset->getGuid().toString() },
        {       "min", { worldBounds.getMin().x, worldBounds.getMin().y, worldBounds.getMin().z } },
        {       "max", { worldBounds.getMax().x, worldBounds.getMax().y, worldBounds.getMax().z } }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    CHECK(result.at("type") == "Error");
    CHECK(result.at("command") == "GetAssetAABB");
    CHECK(result.at("errorType") == "AssetNotFound");
}

TEST_CASE("GetAssetAABBCommand/VoxelAABB")
{
    Database database;
    auto     voxelAsset = std::make_shared<VoxelAsset>();
    voxelAsset->setName("BoxModel");
    voxelAsset->setAABB(CADacomb::AABB3(glm::vec3(-5, -5, -5), glm::vec3(5, 5, 5)));
    voxelAsset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(10.0, 0.0, 0.0)));
    database.getProject().addAsset(voxelAsset);

    GetAssetAABBCommand cmd;
    cmd.setInput({
        {      "type", "GetAssetAABB" },
        {   "version",              1 },
        { "assetName",     "BoxModel" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetAssetAABB");
    CHECK(result.at("min")[0] == 5.0);
    CHECK(result.at("max")[0] == 15.0);
}
#endif
