#include "Commands/Project/GetAssetCountCommand.h"

#include "Database/Database.h"

GetAssetCountCommand::GetAssetCountCommand()
{
}

GetAssetCountCommand::~GetAssetCountCommand()
{
}

std::string GetAssetCountCommand::getType() const
{
    return "GetAssetCount";
}

int GetAssetCountCommand::getVersion() const
{
    return 1;
}

std::string GetAssetCountCommand::getDescription() const
{
    return "Command: GetAssetCount\n"
           "Version: 1\n"
           "Purpose: Queries the total number of assets in the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetCount\",\n"
           "  \"version\": 1\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetCount\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"count\": <size_t>\n"
           "}";
}

nlohmann::json GetAssetCountCommand::execute(Database& database)
{
    size_t count = database.getProject().getAssetCount();

    return createResponse("Success", {
        { "count", count }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetCountCommand/EmptyProject")
{
    Database             database;
    GetAssetCountCommand cmd;

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("type") == "GetAssetCount");
    CHECK(result.at("version") == 1);
    CHECK(result.at("count") == 0);
}

TEST_CASE("GetAssetCountCommand/WithAssets")
{
    Database database;
    auto     asset = std::make_shared<VoxelAsset>();
    asset->setName("CountedAsset");
    database.getProject().addAsset(asset);

    GetAssetCountCommand cmd;
    nlohmann::json       result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(result.at("count") == 1);
}
#endif
