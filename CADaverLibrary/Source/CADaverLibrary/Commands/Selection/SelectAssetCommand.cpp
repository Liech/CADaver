#include "Commands/Selection/SelectAssetCommand.h"

#include "Database/Database.h"
#include "Database/Selection/SelectionEntry.h"
#include "Database/Selection/SubObjectSelection.h"
#include "Database/Selection/SubObjectSelectionFactory.h"

SelectAssetCommand::SelectAssetCommand() {}

SelectAssetCommand::~SelectAssetCommand() {}

std::string SelectAssetCommand::getType() const
{
    return "SelectAsset";
}

int SelectAssetCommand::getVersion() const
{
    return 1;
}

std::string SelectAssetCommand::getDescription() const
{
    return "Command: SelectAsset\n"
           "Version: 1\n"
           "Purpose: Selects or deselects an asset or subasset in the database, optionally with subobject selection details.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SelectAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"mode\": \"Replace\" | \"Add\" | \"Remove\",\n"
           "  \"subObjects\": <object|null>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SelectAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"InvalidMode\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

void SelectAssetCommand::setInput(const Polyglot::Node& node)
{
    assetGuid = CADacomb::GUID(node["assetGuid"].asString());
    mode      = node["mode"].asString();
    if (node.contains("subObjects"))
    {
        subObjectsJson = node["subObjects"].toJson();
    }
    else
    {
        subObjectsJson = nullptr;
    }
}

nlohmann::json SelectAssetCommand::execute(Database& database)
{
    if (!database.getProject().hasAsset(assetGuid))
    {
        return createResponse("AssetNotFound", {
            { "assetGuid", assetGuid.toString() }
        });
    }

    if (mode == "Remove")
    {
        database.getSelection().deselect(assetGuid);
        return createResponse("Success", {
            { "assetGuid", assetGuid.toString() }
        });
    }

    if (mode != "Replace" && mode != "Add")
    {
        return createResponse("InvalidMode", {
            { "assetGuid", assetGuid.toString() },
            {      "mode",                 mode }
        });
    }

    SelectionEntry entry;
    entry.setTargetGuid(assetGuid);
    if (subObjectsJson.is_array())
    {
        for (const auto& item : subObjectsJson)
        {
            entry.addSubObject(SubObjectSelectionFactory::fromJson(item));
        }
    }
    else if (subObjectsJson.is_object())
    {
        entry.addSubObject(SubObjectSelectionFactory::fromJson(subObjectsJson));
    }

    if (mode == "Add")
    {
        database.getSelection().addSelection(entry);
    }
    else
    {
        database.getSelection().select(entry);
    }

    return createResponse("Success", {
        { "assetGuid", assetGuid.toString() }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Polyglot/Node.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("SelectAssetCommand/ReplaceAddRemoveAndInvalidMode")
{
    Database database;
    auto     assetA = std::make_shared<TriangulationAsset>();
    auto     assetB = std::make_shared<TriangulationAsset>();
    database.getProject().addAsset(assetA);
    database.getProject().addAsset(assetB);

    SelectAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {  "assetGuid", assetA->getGuid().toString() },
      {       "mode",                    "Replace" },
      { "subObjects",                      nullptr }
    }));
    CHECK(cmd.execute(database).at("status").get<std::string>() == "Success");
    CHECK(database.getSelection().isSelected(assetA->getGuid()));

    cmd.setInput(Polyglot::Node({
      {  "assetGuid", assetB->getGuid().toString() },
      {       "mode",                        "Add" },
      { "subObjects",                      nullptr }
    }));
    CHECK(cmd.execute(database).at("status").get<std::string>() == "Success");
    CHECK(database.getSelection().isSelected(assetA->getGuid()));
    CHECK(database.getSelection().isSelected(assetB->getGuid()));

    cmd.setInput(Polyglot::Node({
      {  "assetGuid", assetA->getGuid().toString() },
      {       "mode",                     "Remove" },
      { "subObjects",                      nullptr }
    }));
    CHECK(cmd.execute(database).at("status").get<std::string>() == "Success");
    CHECK(!database.getSelection().isSelected(assetA->getGuid()));
    CHECK(database.getSelection().isSelected(assetB->getGuid()));

    cmd.setInput(Polyglot::Node({
      {  "assetGuid", assetB->getGuid().toString() },
      {       "mode",                    "Unknown" },
      { "subObjects",                      nullptr }
    }));
    CHECK(cmd.execute(database).at("status").get<std::string>() == "InvalidMode");
}
#endif
