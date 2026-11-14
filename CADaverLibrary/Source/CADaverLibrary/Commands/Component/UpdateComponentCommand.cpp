#include "Commands/Component/UpdateComponentCommand.h"

#include "Database/Asset.h"
#include "Database/AssetComponent.h"
#include "Database/Database.h"
#include "Database/Project.h"

UpdateComponentCommand::UpdateComponentCommand() {}

UpdateComponentCommand::~UpdateComponentCommand() {}

std::string UpdateComponentCommand::getType() const
{
    return "UpdateComponent";
}

int UpdateComponentCommand::getVersion() const
{
    return 1;
}

std::string UpdateComponentCommand::getDescription() const
{
    return "Command: UpdateComponent\n"
           "Version: 1\n"
           "Purpose: Updates an existing component's name and/or serialized data payload.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateComponent\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": \"<string>\",\n"
           "  \"componentGuid\": \"<string>\",\n"
           "  \"name\": \"<string, optional>\",\n"
           "  \"data\": <object, optional>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"UpdateComponent\",\n"
           "  \"version\": 1\n"
           "}";
}

void UpdateComponentCommand::setInput(const Polyglot::Node& node)
{
    assetGuid     = CADacomb::GUID(node["assetGuid"].asString());
    componentGuid = CADacomb::GUID(node["componentGuid"].asString());
    hasName       = node.contains("name") && !node["name"].isNull();
    if (hasName)
    {
        name = node["name"].asString();
    }
    hasData = node.contains("data") && !node["data"].isNull();
    if (hasData)
    {
        data = node["data"].toJson();
    }
}

nlohmann::json UpdateComponentCommand::execute(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return createErrorResponse("AssetNotFound", "Asset not found: " + assetGuid.toString());
    }

    auto component = asset->getComponent(componentGuid);
    if (!component)
    {
        return createErrorResponse("ComponentNotFound", "Component not found: " + componentGuid.toString());
    }

    previousJson = component->toJson();

    if (hasName)
    {
        component->setName(name);
    }
    if (hasData)
    {
        component->fromDataJson(data);
    }

    updatedJson = component->toJson();
    return createResponse();
}

bool UpdateComponentCommand::isUndoable() const
{
    return true;
}

void UpdateComponentCommand::undo(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (asset)
    {
        auto component = asset->getComponent(componentGuid);
        if (component)
        {
            component->fromJson(previousJson);
        }
    }
}

void UpdateComponentCommand::redo(Database& database)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (asset)
    {
        auto component = asset->getComponent(componentGuid);
        if (component)
        {
            component->fromJson(updatedJson);
        }
    }
}

size_t UpdateComponentCommand::getUndoSize() const
{
    return sizeof(UpdateComponentCommand) + previousJson.dump().size() + updatedJson.dump().size();
}

#ifdef ISTESTPROJECT
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Component/TextNoteComponent.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("UpdateComponentCommand/UpdatesAndRestoresOnUndoRedo")
{
    Database database;
    auto     triAsset = std::make_shared<TriangulationAsset>();
    auto     note     = std::make_shared<TextNoteComponent>("Initial");
    note->setName("Original");
    triAsset->addComponent(note);
    database.getProject().addAsset(triAsset);

    UpdateComponentCommand cmd;
    cmd.setInput(Polyglot::Node({
      {     "assetGuid", triAsset->getGuid().toString() },
      { "componentGuid",     note->getGuid().toString() },
      {          "name",                      "Renamed" },
      {          "data",      { { "Version", 1 }, { "text", "Updated" } } }
    }));

    nlohmann::json res = cmd.execute(database);
    CHECK(res.at("type").get<std::string>() == "UpdateComponent");
    CHECK(note->getName() == "Renamed");
    CHECK(note->getText() == "Updated");

    cmd.undo(database);
    CHECK(note->getName() == "Original");
    CHECK(note->getText() == "Initial");

    cmd.redo(database);
    CHECK(note->getName() == "Renamed");
    CHECK(note->getText() == "Updated");
}
#endif
