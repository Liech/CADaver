#include "Commands/Project/RemoveAssetCommand.h"

#include "Database/Asset.h"
#include "Database/Database.h"

RemoveAssetCommand::RemoveAssetCommand() {}

RemoveAssetCommand::~RemoveAssetCommand() {}

std::string RemoveAssetCommand::getType() const
{
    return "RemoveAsset";
}

int RemoveAssetCommand::getVersion() const
{
    return 1;
}

std::string RemoveAssetCommand::getDescription() const
{
    return "Command: RemoveAsset\n"
           "Version: 1\n"
           "Purpose: Removes an asset from the project by GUID.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"RemoveAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"RemoveAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\",\n"
           "  \"assetGuid\": <string>\n"
           "}";
}

void RemoveAssetCommand::setInput(const nlohmann::json& json)
{
    assetGuid = json.value("assetGuid", "");
}

nlohmann::json RemoveAssetCommand::execute(Database& database, std::istream*, std::ostream*)
{
    removedAsset = database.getProject().getAsset(assetGuid);
    if (!removedAsset)
    {
        return {
            {      "type",            getType() },
            {   "version",         getVersion() },
            {    "status",      "AssetNotFound" },
            { "assetGuid", assetGuid.toString() }
        };
    }

    database.getProject().removeAsset(assetGuid);

    return {
        {      "type",            getType() },
        {   "version",         getVersion() },
        {    "status",            "Success" },
        { "assetGuid", assetGuid.toString() }
    };
}

bool RemoveAssetCommand::isUndoable() const
{
    return removedAsset != nullptr;
}

void RemoveAssetCommand::undo(Database& database)
{
    if (removedAsset)
    {
        database.getProject().addAsset(removedAsset);
    }
}

void RemoveAssetCommand::redo(Database& database)
{
    if (removedAsset)
    {
        database.getProject().removeAsset(removedAsset->getGuid());
    }
}

size_t RemoveAssetCommand::getUndoSize() const
{
    return sizeof(RemoveAssetCommand) + (removedAsset ? removedAsset->getSizeInBytes() : 0);
}
