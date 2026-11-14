#include "Commands/IO/SaveAssetCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"
#include "IO/AssetSaver/AssetSaver.h"
#include "IO/AssetSaver/AssetSaverFactory.h"

SaveAssetCommand::SaveAssetCommand() {}

SaveAssetCommand::~SaveAssetCommand() {}

std::string SaveAssetCommand::getType() const
{
    return "SaveAsset";
}

int SaveAssetCommand::getVersion() const
{
    return 1;
}

std::string SaveAssetCommand::getDescription() const
{
    return "Command: SaveAsset\n"
           "Version: 1\n"
           "Purpose: Saves any supported project asset (Cad, Triangulation, Voxel) to a destination callback or file path.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"destination\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveAsset\",\n"
           "  \"version\": 1\n"
           "}";
}

std::vector<CADacomb::GUID> SaveAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SaveAssetCommand::setInput(const Polyglot::Node& node)
{
    assetGuid   = node["assetGuid"].asString();
    destination = node["destination"];
}

nlohmann::json SaveAssetCommand::execute(Database& database)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    AssetSaverFactory factory;
    auto              saver = factory.create(baseAsset->getType());
    if (!saver)
    {
        return createErrorResponse("UnsupportedType", "Unsupported asset type: " + baseAsset->getType());
    }

    if (!saver->save(*baseAsset, destination))
    {
        return createErrorResponse("WriteFailed", "Failed to save asset: " + baseAsset->getName());
    }

    return createResponse();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveAssetCommand/TypeAndVersion")
{
    CADacomb::GUID   guid = CADacomb::GUID::random();
    SaveAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {   "assetGuid", guid.toString() },
      { "destination",    "output.stl" }
    }));
    CHECK(cmd.getType() == "SaveAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
