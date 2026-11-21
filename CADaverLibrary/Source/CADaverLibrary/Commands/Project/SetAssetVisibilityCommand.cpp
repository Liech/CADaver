#include "Commands/Project/SetAssetVisibilityCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"

SetAssetVisibilityCommand::SetAssetVisibilityCommand() {}

SetAssetVisibilityCommand::~SetAssetVisibilityCommand() {}

std::string SetAssetVisibilityCommand::getType() const
{
    return "SetAssetVisibility";
}

int SetAssetVisibilityCommand::getVersion() const
{
    return 1;
}

std::string SetAssetVisibilityCommand::getDescription() const
{
    return "Command: SetAssetVisibility\n"
           "Version: 1\n"
           "Purpose: Sets the visibility state of an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SetAssetVisibility\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"visible\": <bool>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SetAssetVisibility\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"visible\": <bool>\n"
           "}";
}

std::vector<CADacomb::GUID> SetAssetVisibilityCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SetAssetVisibilityCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = CADacomb::GUID(node["assetGuid"].asString());
    visible   = node["visible"].asBool();
}

nlohmann::json SetAssetVisibilityCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    previousVisible = asset->getVisible();
    asset->setVisible(visible);

    return createResponse({
        { "assetGuid", assetGuid.toString() },
        {   "visible",              visible }
    });
}

bool SetAssetVisibilityCommand::isUndoable() const
{
    return true;
}

void SetAssetVisibilityCommand::undo(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (asset)
    {
        asset->setVisible(previousVisible);
    }
}

void SetAssetVisibilityCommand::redo(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (asset)
    {
        asset->setVisible(visible);
    }
}

size_t SetAssetVisibilityCommand::getUndoSize() const
{
    return sizeof(SetAssetVisibilityCommand);
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SetAssetVisibilityCommand/SetsVisibilityAndSupportsUndoRedo")
{
    Database database;
    auto     asset = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(asset);
    CHECK(asset->getVisible() == true);

    SetAssetVisibilityCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", asset->getGuid().toString() },
      {   "visible",                       false }
    }));

    size_t         initialRev = asset->getRevision();
    nlohmann::json res        = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "SetAssetVisibility");
    CHECK(res.at("assetGuid").get<std::string>() == asset->getGuid().toString());
    CHECK(res.at("visible").get<bool>() == false);
    CHECK(asset->getVisible() == false);
    CHECK(asset->getRevision() > initialRev);

    cmd.undo(database);
    CHECK(asset->getVisible() == true);

    cmd.redo(database);
    CHECK(asset->getVisible() == false);
}

TEST_CASE("SetAssetVisibilityCommand/AssetNotFound")
{
    Database                  database;
    SetAssetVisibilityCommand cmd;
    cmd.setInput(Polyglot::Node({
      { "assetGuid", CADacomb::GUID::random().toString() },
      {   "visible",                               false }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "Error");
    CHECK(res.at("command").get<std::string>() == "SetAssetVisibility");
    CHECK(res.at("errorType").get<std::string>() == "AssetNotFound");
}
#endif
