#include "Commands/Component/GetNotesCommand.h"

#include "Database/Asset.h"
#include "Database/Component/TextNoteComponent.h"
#include "Database/Database.h"

GetNotesCommand::GetNotesCommand() {}

GetNotesCommand::~GetNotesCommand() {}

std::string GetNotesCommand::getType() const
{
    return "GetNotes";
}

int GetNotesCommand::getVersion() const
{
    return 1;
}

std::string GetNotesCommand::getDescription() const
{
    return "Command: GetNotes\n"
           "Version: 1\n"
           "Purpose: Queries all text note components attached to a specific asset.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetNotes\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"GetNotes\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\",\n"
           "  \"assetGuid\": <string>,\n"
           "  \"notes\": [\n"
           "    {\n"
           "      \"componentGuid\": <string>,\n"
           "      \"name\": <string>,\n"
           "      \"text\": <string>\n"
           "    }\n"
           "  ]\n"
           "}";
}

void GetNotesCommand::setInput(const Polyglot::Node& node)
{
    if (node.contains("assetGuid"))
    {
        assetGuid = CADacomb::GUID(node["assetGuid"].asString());
    }
}

nlohmann::json GetNotesCommand::execute(Database& database)
{
    nlohmann::json notesArray = nlohmann::json::array();
    auto           asset      = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createResponse("AssetNotFound",
                              {
                                { "assetGuid", assetGuid.toString() },
                                {     "notes",           notesArray }
        });
    }

    auto notes = asset->getAttachments<TextNoteComponent>();
    for (const auto& note : notes)
    {
        if (!note)
        {
            continue;
        }

        notesArray.push_back({
          { "componentGuid", note->getGuid().toString() },
          {          "name",            note->getName() },
          {          "text",            note->getText() }
        });
    }

    return createResponse("Success",
                          {
                            { "assetGuid", asset->getGuid().toString() },
                            {     "notes",                  notesArray }
    });
}

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GetNotesCommand/AssetNotFound")
{
    Database        database;
    GetNotesCommand cmd;
    cmd.setInput({
      {      "type",                          "GetNotes" },
      {   "version",                                   1 },
      { "assetGuid", CADacomb::GUID::random().toString() }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "AssetNotFound");
    CHECK(result.at("notes").empty());
}

TEST_CASE("GetNotesCommand/ReturnsAttachedNotes")
{
    Database database;
    auto     asset = std::make_shared<VoxelAsset>();
    auto     note  = std::make_shared<TextNoteComponent>();
    note->setName("Header");
    note->setText("Body content");
    asset->addComponent(note);
    database.getProject().addAsset(asset);

    GetNotesCommand cmd;
    cmd.setInput({
      {      "type",                  "GetNotes" },
      {   "version",                           1 },
      { "assetGuid", asset->getGuid().toString() }
    });

    nlohmann::json result = cmd.execute(database);
    CHECK(result.at("status") == "Success");
    REQUIRE(result.at("notes").size() == 1);
    CHECK(result.at("notes")[0].at("name") == "Header");
    CHECK(result.at("notes")[0].at("text") == "Body content");
}
#endif
