#include "Commands/Project/GetAssetVisibilityCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"

GetAssetVisibilityCommand::GetAssetVisibilityCommand() {}

GetAssetVisibilityCommand::~GetAssetVisibilityCommand() {}

std::string GetAssetVisibilityCommand::getType() const
{
    return "GetAssetVisibility";
}

int GetAssetVisibilityCommand::getVersion() const
{
    return 1;
}

std::string GetAssetVisibilityCommand::getDescription() const
{
    return "Command: GetAssetVisibility\n"
           "Version: 1\n"
           "Purpose: Queries the visibility state of an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetVisibility\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetAssetVisibility\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"visible\": <bool>\n"
           "}";
}

void GetAssetVisibilityCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = CADacomb::GUID(node["assetGuid"].asString());
}

nlohmann::json GetAssetVisibilityCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    return createResponse({
        { "assetGuid", assetGuid.toString() },
        {   "visible",  asset->getVisible() }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetAssetVisibilityCommand/ReturnsCurrentVisibility")
{
    Database database;
    auto     asset = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(asset);

    GetAssetVisibilityCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", asset->getGuid().toString() }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "GetAssetVisibility");
    CHECK(res.at("assetGuid").get<std::string>() == asset->getGuid().toString());
    CHECK(res.at("visible").get<bool>() == true);

    asset->setVisible(false);
    nlohmann::json resHidden = cmd.execute(database);
    CHECK(resHidden.at("visible").get<bool>() == false);
}

TEST_CASE("GetAssetVisibilityCommand/AssetNotFound")
{
    Database                  database;
    GetAssetVisibilityCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", CADacomb::GUID::random().toString() }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "Error");
    CHECK(res.at("command").get<std::string>() == "GetAssetVisibility");
    CHECK(res.at("errorType").get<std::string>() == "AssetNotFound");
}
#endif
