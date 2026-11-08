#include "Commands/Project/GetAssetNamesCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"

GetAssetNamesCommand::GetAssetNamesCommand()
{
}

GetAssetNamesCommand::~GetAssetNamesCommand()
{
}

std::string GetAssetNamesCommand::getType() const
{
    return "GetAssetNames";
}

int GetAssetNamesCommand::getVersion() const
{
    return 1;
}

std::string GetAssetNamesCommand::getDescription() const
{
    return "Command: GetAssetNames\n"
           "Version: 1\n"
           "Purpose: Queries all asset names currently in the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetNames\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetNames\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"assetNames\": [<string>]\n"
           "}";
}

nlohmann::json GetAssetNamesCommand::execute(Database& database)
{
    std::vector<std::string> assetNames;
    const auto&              assets = database.getProject().getAssets();
    assetNames.reserve(assets.size());
    for (const auto& asset : assets)
    {
        if (asset)
        {
            assetNames.push_back(asset->getName());
        }
    }

    return createResponse("Success", {
        { "assetNames", assetNames }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetNamesCommand/EmptyProject")
{
    Database             database;
    GetAssetNamesCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("type") == "GetAssetNames");
    CHECK(result.at("version") == 1);
    CHECK(result.at("assetNames").empty());
}

TEST_CASE("GetAssetNamesCommand/WithAssets")
{
    Database database;
    auto     asset1 = std::make_shared<VoxelAsset>();
    asset1->setName("FirstAsset");
    auto asset2 = std::make_shared<VoxelAsset>();
    asset2->setName("SecondAsset");

    database.getProject().addAsset(asset1);
    database.getProject().addAsset(asset2);

    GetAssetNamesCommand cmd;
    nlohmann::json       result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    std::vector<std::string> names = result.at("assetNames").get<std::vector<std::string>>();
    REQUIRE(names.size() == 2);
    CHECK(names[0] == "FirstAsset");
    CHECK(names[1] == "SecondAsset");
}
#endif
