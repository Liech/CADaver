#include "Commands/Component/UpdateNoteCommand.h"

#include "Database/Asset.h"
#include "Database/Component/TextNoteComponent.h"
#include "Database/Database.h"

UpdateNoteCommand::UpdateNoteCommand() {}

UpdateNoteCommand::~UpdateNoteCommand() {}

std::string UpdateNoteCommand::getType() const
{
    return "UpdateNote";
}

int UpdateNoteCommand::getVersion() const
{
    return 1;
}

std::string UpdateNoteCommand::getDescription() const
{
    return "Command: UpdateNote\n"
           "Version: 1\n"
           "Purpose: Updates the title and text body of an existing text note component on an asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateNote\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string>,\n"
           "  \"name\": <string>,\n"
           "  \"text\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateNote\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"ComponentNotFound\",\n"
           "  \"assetGuid\": <string>,\n"
           "  \"componentGuid\": <string>\n"
           "}";
}

void UpdateNoteCommand::setInput(const Polyglot::Node& node)
{
    assetGuid     = CADacomb::GUID(node["assetGuid"].asString());
    componentGuid = CADacomb::GUID(node["componentGuid"].asString());
    newName       = node["name"].asString();
    newText       = node["text"].asString();
}

nlohmann::json UpdateNoteCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createResponse("AssetNotFound", {
                                                 {     "assetGuid",     assetGuid.toString() },
                                                 { "componentGuid", componentGuid.toString() }
                                               });
    }

    auto notes = asset->getAttachments<TextNoteComponent>();
    for (const auto& note : notes)
    {
        if (note && note->getGuid() == componentGuid)
        {
            previousName = note->getName();
            previousText = note->getText();
            note->setName(newName);
            note->setText(newText);

            return createResponse("Success", {
                                               {     "assetGuid",     assetGuid.toString() },
                                               { "componentGuid", componentGuid.toString() }
                                             });
        }
    }

    return createResponse("ComponentNotFound", {
                                                 {     "assetGuid",     assetGuid.toString() },
                                                 { "componentGuid", componentGuid.toString() }
                                               });
}

bool UpdateNoteCommand::isUndoable() const
{
    return true;
}

void UpdateNoteCommand::undo(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return;
    }

    auto notes = asset->getAttachments<TextNoteComponent>();
    for (const auto& note : notes)
    {
        if (note && note->getGuid() == componentGuid)
        {
            note->setName(previousName);
            note->setText(previousText);
            return;
        }
    }
}

void UpdateNoteCommand::redo(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return;
    }

    auto notes = asset->getAttachments<TextNoteComponent>();
    for (const auto& note : notes)
    {
        if (note && note->getGuid() == componentGuid)
        {
            note->setName(newName);
            note->setText(newText);
            return;
        }
    }
}

size_t UpdateNoteCommand::getUndoSize() const
{
    return sizeof(UpdateNoteCommand) + newName.size() + newText.size() + previousName.size() + previousText.size();
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UpdateNoteCommand/ExecuteUndoRedo")
{
    Database database;
    auto     asset = std::make_shared<VoxelAsset>();
    auto     note  = std::make_shared<TextNoteComponent>();
    note->setName("OldName");
    note->setText("OldText");
    CADacomb::GUID compGuid = asset->addComponent(note);
    database.getProject().addAsset(asset);

    UpdateNoteCommand cmd;
    cmd.setInput({
      {          "type",                "UpdateNote" },
      {       "version",                           1 },
      {     "assetGuid", asset->getGuid().toString() },
      { "componentGuid",         compGuid.toString() },
      {          "name",                   "NewName" },
      {          "text",                   "NewText" }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    CHECK(note->getName() == "NewName");
    CHECK(note->getText() == "NewText");

    cmd.undo(database);
    CHECK(note->getName() == "OldName");
    CHECK(note->getText() == "OldText");

    cmd.redo(database);
    CHECK(note->getName() == "NewName");
    CHECK(note->getText() == "NewText");
}
#endif
