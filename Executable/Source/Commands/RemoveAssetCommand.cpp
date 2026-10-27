#include "RemoveAssetCommand.h"

#include "Commands/Undo/RemoveAssetUndoRecord.h"
#include "Database/Database.h"

RemoveAssetCommand::RemoveAssetCommand() {}

RemoveAssetCommand::RemoveAssetCommand(int assetIndex)
  : assetIndex(assetIndex)
{
}

RemoveAssetCommand::RemoveAssetCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
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
           "Purpose: Removes an asset from the project by index.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"RemoveAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"RemoveAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"IndexOutOfBounds\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json RemoveAssetCommand::execute(Database& database, std::istream*, std::ostream*, bool recordUndo)
{
    if (assetIndex < 0 || static_cast<size_t>(assetIndex) >= database.getProject().getAssetCount())
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "IndexOutOfBounds" },
            {   "index",         assetIndex }
        };
    }

    auto asset = database.getProject().getAsset(static_cast<size_t>(assetIndex));
    database.getProject().removeAsset(static_cast<size_t>(assetIndex));

    if (recordUndo && asset)
    {
        database.getUndoStack().push(std::make_unique<RemoveAssetUndoRecord>(asset, static_cast<size_t>(assetIndex)));
    }

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",   assetIndex }
    };
}

int RemoveAssetCommand::getAssetIndex() const
{
    return assetIndex;
}

void RemoveAssetCommand::setAssetIndex(int index)
{
    assetIndex = index;
}
