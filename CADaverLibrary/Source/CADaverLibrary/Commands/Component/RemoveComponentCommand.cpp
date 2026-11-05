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
           "  \"assetGuid\": \"<string, optional>\"\n"
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
    nlohmann::json json = node.toJson();
    componentGuid       = CADacomb::GUID(json.at("componentGuid").get<std::string>());
    if (json.contains("assetGuid"))
    {
        assetGuid = CADacomb::GUID(json.at("assetGuid").get<std::string>());
    }
}

nlohmann::json RemoveComponentCommand::execute(Database& database)
{
    std::shared_ptr<Asset> targetAsset;

    if (!assetGuid.toString().empty())
    {
        targetAsset = database.getProject().getAsset(assetGuid);
    }
    else
    {
        for (const auto& asset : database.getProject().getAssets())
        {
            if (asset && asset->getComponent(componentGuid))
            {
                targetAsset = asset;
                assetGuid   = asset->getGuid();
                break;
            }
        }
    }

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
    if (removedComponent && !assetGuid.toString().empty())
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
    if (removedComponent && !assetGuid.toString().empty())
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
      {          "type",          "RemoveComponent" },
      {       "version",                          1 },
      { "componentGuid", note->getGuid().toString() }
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
