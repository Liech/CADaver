#include "Commands/Project/GetAssetInfoCommand.h"

#include "Database/Asset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

GetAssetInfoCommand::GetAssetInfoCommand() {}

GetAssetInfoCommand::~GetAssetInfoCommand() {}

std::string GetAssetInfoCommand::getType() const
{
    return "GetAssetInfo";
}

int GetAssetInfoCommand::getVersion() const
{
    return 1;
}

std::string GetAssetInfoCommand::getDescription() const
{
    return "Command: GetAssetInfo\n"
           "Version: 1\n"
           "Purpose: Queries detailed metadata for a specific asset by GUID or name.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetInfo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string> (optional),\n"
           "  \"assetName\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetInfo\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"assetName\": <string>,\n"
           "  \"assetType\": <string>,\n"
           "  \"sizeInBytes\": <size_t>,\n"
           "  \"revision\": <size_t>,\n"
           "  \"componentCount\": <size_t>\n"
           "}";
}

void GetAssetInfoCommand::setInput(const Polyglot::Node& node)
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

nlohmann::json GetAssetInfoCommand::execute(Database& database)
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

    uint32_t dimension  = 0;
    auto     voxelAsset = std::dynamic_pointer_cast<const VoxelAsset>(foundAsset);
    if (voxelAsset)
    {
        dimension = voxelAsset->getSVO().getDimension();
    }

    return createResponse({
      {      "assetGuid",    foundAsset->getGuid().toString() },
      {      "assetName",               foundAsset->getName() },
      {      "assetType",               foundAsset->getType() },
      {    "sizeInBytes",        foundAsset->getSizeInBytes() },
      {       "revision",           foundAsset->getRevision() },
      { "componentCount", foundAsset->getAttachments().size() },
      {      "dimension",                           dimension }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetInfoCommand/NotFound")
{
    Database            database;
    GetAssetInfoCommand cmd;
    cmd.setInput({
      {      "type", "GetAssetInfo" },
      {   "version",              1 },
      { "assetName",  "NonExistent" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "Error");
    CHECK(result.at("command") == "GetAssetInfo");
    CHECK(result.at("errorType") == "AssetNotFound");
}

TEST_CASE("GetAssetInfoCommand/FindByGuid")
{
    Database database;
    auto     asset = std::make_shared<VoxelAsset>();
    asset->setName("VoxelModel");
    asset->setSVO(std::make_unique<CADacomb::SVO<bool>>(16, false));
    database.getProject().addAsset(asset);

    GetAssetInfoCommand cmd;
    cmd.setInput({
      {      "type",              "GetAssetInfo" },
      {   "version",                           1 },
      { "assetGuid", asset->getGuid().toString() }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetAssetInfo");
    CHECK(result.at("assetGuid") == asset->getGuid().toString());
    CHECK(result.at("assetName") == "VoxelModel");
    CHECK(result.at("assetType") == "Voxel");
    CHECK(result.at("sizeInBytes") == asset->getSizeInBytes());
    CHECK(result.at("revision") == asset->getRevision());
    CHECK(result.at("componentCount") == 0);
    CHECK(result.at("dimension") == 16);
}

TEST_CASE("GetAssetInfoCommand/FindByName")
{
    Database database;
    auto     asset = std::make_shared<VoxelAsset>();
    asset->setName("NamedModel");
    asset->setSVO(std::make_unique<CADacomb::SVO<bool>>(32, false));
    database.getProject().addAsset(asset);

    GetAssetInfoCommand cmd;
    cmd.setInput({
      {      "type", "GetAssetInfo" },
      {   "version",              1 },
      { "assetName",   "NamedModel" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetAssetInfo");
    CHECK(result.at("assetGuid") == asset->getGuid().toString());
    CHECK(result.at("assetName") == "NamedModel");
    CHECK(result.at("assetType") == "Voxel");
    CHECK(result.at("dimension") == 32);
}
#endif
