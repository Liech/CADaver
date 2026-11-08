#include "Commands/Component/RemoveComponentCommand.h"

#include "Database/Asset.h"
#include "Database/AssetComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"

RemoveComponentCommand::RemoveComponentCommand() {}

RemoveComponentCommand::~RemoveComponentCommand() {}

std::string RemoveComponentCommand::getType() const
{
    return "RemoveComponent";
}

int RemoveComponentCommand::getVersion() const
{
    return 1;
}

std::string RemoveComponentCommand::getDescription() const
{
    return "Command: RemoveComponent\n"
           "Version: 1\n"
           "Purpose: Removes an asset component indiscriminately by GUID.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"RemoveComponent\",\n"
           "  \"version\": 1,\n"
           "  \"componentGuid\": \"<string>\",\n"
           "  \"assetGuid\": \"<string>\"\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"RemoveComponent\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\"\n"
           "}";
}

void RemoveComponentCommand::setInput(const Polyglot::Node& node)
{
    componentGuid = CADacomb::GUID(node["componentGuid"].asString());
    assetGuid     = CADacomb::GUID(node["assetGuid"].asString());
}

nlohmann::json RemoveComponentCommand::execute(Database& database)
{
    auto targetAsset = database.getProject().getAsset(assetGuid);
    if (!targetAsset)
    {
        return {
            {    "type",         getType() },
            { "version",      getVersion() },
            {  "status",          "Failed" },
            {   "error", "Asset not found" }
        };
    }

    removedComponent = targetAsset->getComponent(componentGuid);
    if (!removedComponent)
    {
        return {
            {    "type",             getType() },
            { "version",          getVersion() },
            {  "status",              "Failed" },
            {   "error", "Component not found" }
        };
    }

    targetAsset->removeComponent(componentGuid);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

bool RemoveComponentCommand::isUndoable() const
{
    return true;
}

void RemoveComponentCommand::undo(Database& database)
{
    if (removedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->addComponent(removedComponent);
        }
    }
}

void RemoveComponentCommand::redo(Database& database)
{
    if (removedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->removeComponent(componentGuid);
        }
    }
}

size_t RemoveComponentCommand::getUndoSize() const
{
    return sizeof(RemoveComponentCommand) + (removedComponent ? removedComponent->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include "Database/Component/TextNoteComponent.h"

#include <catch2/catch_test_macros.hpp>

class MockAssetForRemove : public Asset
{
  public:
    MockAssetForRemove() {}
    virtual ~MockAssetForRemove() {}

    std::string getType() const override
    {
        return "Mock";
    }
    size_t getSizeInBytes() const override
    {
        return sizeof(MockAssetForRemove);
    }
};

TEST_CASE("RemoveComponentCommand/ExecuteAndUndo")
{
    Database database;
    auto     asset = std::make_shared<MockAssetForRemove>();
    database.getProject().addAsset(asset);

    auto note = asset->addComponent<TextNoteComponent>();
    note->setText("Remove me");

    RemoveComponentCommand cmd;
    cmd.setInput({
      {          "type",           "RemoveComponent" },
      {       "version",                           1 },
      { "componentGuid",  note->getGuid().toString() },
      {     "assetGuid", asset->getGuid().toString() }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result["status"] == "Success");
    CHECK(asset->getAttachments().empty());

    cmd.undo(database);
    CHECK(asset->getAttachments().size() == 1);

    cmd.redo(database);
    CHECK(asset->getAttachments().empty());
}
#endif
