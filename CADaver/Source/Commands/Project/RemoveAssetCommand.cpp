#include "Commands/Project/RemoveAssetCommand.h"

#include "Commands/Undo/RemoveAssetUndoRecord.h"
#include "Commands/Undo/UndoStack.h"
#include "Database/Database.h"

RemoveAssetCommand::RemoveAssetCommand() {}

RemoveAssetCommand::RemoveAssetCommand(const CADacomb::GUID& assetGuid)
  : assetGuid(assetGuid)
{
}

RemoveAssetCommand::RemoveAssetCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
{
}

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

nlohmann::json RemoveAssetCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    auto asset = database.getProject().getAsset(assetGuid);
    if (!asset)
    {
        return {
            {      "type",            getType() },
            {   "version",         getVersion() },
            {    "status",      "AssetNotFound" },
            { "assetGuid", assetGuid.toString() }
        };
    }

    database.getProject().removeAsset(assetGuid);

    if (recordUndo)
    {
        UndoStack::getInstance().push(std::make_unique<RemoveAssetUndoRecord>(asset));
    }

    return {
        {      "type",            getType() },
        {   "version",         getVersion() },
        {    "status",            "Success" },
        { "assetGuid", assetGuid.toString() }
    };
}

const CADacomb::GUID& RemoveAssetCommand::getAssetGuid() const
{
    return assetGuid;
}

void RemoveAssetCommand::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}
