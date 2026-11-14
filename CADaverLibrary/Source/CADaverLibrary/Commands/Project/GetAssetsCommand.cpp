#include "Commands/Project/GetAssetsCommand.h"

#include "Database/Asset.h"
#include "Database/Asset/ProgressBarAsset.h"
#include "Database/AssetComponent.h"
#include "Database/Database.h"

GetAssetsCommand::GetAssetsCommand() {}

GetAssetsCommand::~GetAssetsCommand() {}

std::string GetAssetsCommand::getType() const
{
    return "GetAssets";
}

int GetAssetsCommand::getVersion() const
{
    return 1;
}

std::string GetAssetsCommand::getDescription() const
{
    return "Command: GetAssets\n"
           "Version: 1\n"
           "Purpose: Queries all assets in the project along with their metadata and attached component summaries.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssets\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssets\",\n"
           "  \"version\": 1,\n"
           "  \"assets\": [\n"
           "    {\n"
           "      \"assetGuid\": <string>,\n"
           "      \"name\": <string>,\n"
           "      \"type\": <string>,\n"
           "      \"revision\": <size_t>,\n"
           "      \"components\": [\n"
           "        {\n"
           "          \"componentGuid\": <string>,\n"
           "          \"name\": <string>,\n"
           "          \"type\": <string>\n"
           "        }\n"
           "      ]\n"
           "    }\n"
           "  ]\n"
           "}";
}

nlohmann::json GetAssetsCommand::execute(Database& database)
{
    nlohmann::json assetArray = nlohmann::json::array();
    const auto&    assets     = database.getProject().getAssets();

    for (const auto& asset : assets)
    {
        if (!asset)
        {
            continue;
        }

        nlohmann::json componentArray = nlohmann::json::array();
        for (const auto& comp : asset->getAttachments())
        {
            if (!comp)
            {
                continue;
            }

            componentArray.push_back({
              { "componentGuid", comp->getGuid().toString() },
              {          "name",            comp->getName() },
              {          "type",            comp->getType() }
            });
        }

        nlohmann::json assetJson = {
            {  "assetGuid", asset->getGuid().toString() },
            {       "name",            asset->getName() },
            {       "type",            asset->getType() },
            {   "revision",        asset->getRevision() },
            { "components",              componentArray }
        };

        auto progressAsset = std::dynamic_pointer_cast<const ProgressBarAsset>(asset);
        if (progressAsset)
        {
            assetJson["operationName"] = progressAsset->getOperationName();
            assetJson["progress"]      = progressAsset->getProgress();
            assetJson["isReady"]       = progressAsset->isReady();
            if (progressAsset->isReady())
            {
                Polyglot::Node res = progressAsset->getResult();
                if (res.contains("type") && res["type"].isString() && res["type"].asString() == "Error")
                {
                    assetJson["taskStatus"] = res.contains("errorType") && res["errorType"].isString() ? res["errorType"].asString() : "Error";
                    assetJson["taskError"]  = res.contains("message") && res["message"].isString() ? res["message"].asString() : "";
                }
                else
                {
                    assetJson["taskStatus"] = "Success";
                    assetJson["taskError"]  = "";
                }
            }
        }

        assetArray.push_back(assetJson);
    }

    return createResponse({
      { "assets", assetArray }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include "Database/Component/TextNoteComponent.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetsCommand/EmptyProject")
{
    Database         database;
    GetAssetsCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("type") == "GetAssets");
    CHECK(result.at("assets").empty());
}

TEST_CASE("GetAssetsCommand/WithAssetAndComponent")
{
    Database database;
    auto     asset = std::make_shared<VoxelAsset>();
    asset->setName("Terrain");
    auto note = std::make_shared<TextNoteComponent>();
    note->setName("Info");
    asset->addComponent(note);
    database.getProject().addAsset(asset);

    GetAssetsCommand cmd;
    nlohmann::json   result = cmd.execute(database);
    CHECK(result.at("type") == "GetAssets");
    REQUIRE(result.at("assets").size() == 1);
    CHECK(result.at("assets")[0].at("name") == "Terrain");
    CHECK(result.at("assets")[0].at("type") == "Voxel");
    REQUIRE(result.at("assets")[0].at("components").size() == 1);
    CHECK(result.at("assets")[0].at("components")[0].at("name") == "Info");
}
#endif
