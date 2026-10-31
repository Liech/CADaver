#include "Commands/Component/AddNoteCommand.h"

#include "Database/Asset.h"
#include "Database/Component/TextNoteComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"

AddNoteCommand::AddNoteCommand() {}

AddNoteCommand::~AddNoteCommand() {}

std::string AddNoteCommand::getType() const
{
    return "AddNote";
}

int AddNoteCommand::getVersion() const
{
    return 1;
}

std::string AddNoteCommand::getDescription() const
{
    return "Command: AddNote\n"
           "Version: 1\n"
           "Purpose: Adds a text note component to an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"AddNote\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"name\": \"<string, optional>\",\n"
           "  \"text\": \"<string, optional>\"\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"AddNote\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"componentGuid\": \"<string>\"\n"
           "}";
}

void AddNoteCommand::setInput(const nlohmann::json& json)
{
    assetGuid = CADacomb::GUID(json.at("assetGuid").get<std::string>());
    if (json.contains("name"))
    {
        name = json.at("name").get<std::string>();
    }
    if (json.contains("text"))
    {
        text = json.at("text").get<std::string>();
    }
}

nlohmann::json AddNoteCommand::execute(Database& database, std::istream*, std::ostream*)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return {
            {    "type",         getType() },
            { "version",      getVersion() },
            {  "status",          "Failed" },
            {   "error", "Asset not found" }
        };
    }

    addedComponent = std::make_shared<TextNoteComponent>();
    addedComponent->setName(name);
    addedComponent->setText(text);

    CADacomb::GUID compGuid = asset->addComponent(addedComponent);
    if (compGuid.toString().empty())
    {
        return {
            {    "type",                           getType() },
            { "version",                        getVersion() },
            {  "status",                            "Failed" },
            {   "error", "Component incompatible with asset" }
        };
    }

    return {
        {          "type",           getType() },
        {       "version",        getVersion() },
        {        "status",           "Success" },
        { "componentGuid", compGuid.toString() }
    };
}

bool AddNoteCommand::isUndoable() const
{
    return true;
}

void AddNoteCommand::undo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->removeComponent(addedComponent->getGuid());
        }
    }
}

void AddNoteCommand::redo(Database& database)
{
    if (addedComponent)
    {
        auto asset = database.getProject().getAsset(assetGuid);
        if (asset)
        {
            asset->addComponent(addedComponent);
        }
    }
}

size_t AddNoteCommand::getUndoSize() const
{
    return sizeof(AddNoteCommand) + (addedComponent ? addedComponent->getSizeInBytes() : 0);
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

class TestAssetForNote : public Asset
{
  public:
    TestAssetForNote() {}
    virtual ~TestAssetForNote() {}

    std::string getType() const override
    {
        return "TestAsset";
    }
    size_t getSizeInBytes() const override
    {
        return sizeof(TestAssetForNote);
    }
};

TEST_CASE("AddNoteCommand/ExecuteAndUndo")
{
    Database database;
    auto     asset = std::make_shared<TestAssetForNote>();
    database.getProject().addAsset(asset);

    AddNoteCommand cmd;
    cmd.setInput({
      {      "type",                   "AddNote" },
      {   "version",                           1 },
      { "assetGuid", asset->getGuid().toString() },
      {      "name",                   "My Note" },
      {      "text",               "Hello World" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result["status"] == "Success");
    CHECK(asset->getAttachments().size() == 1);

    cmd.undo(database);
    CHECK(asset->getAttachments().empty());

    cmd.redo(database);
    CHECK(asset->getAttachments().size() == 1);
}
#endif
