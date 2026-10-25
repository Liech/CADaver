#include "SelectAssetCommand.h"

#include "Database/Database.h"

SelectAssetCommand::SelectAssetCommand() {}

SelectAssetCommand::SelectAssetCommand(int index)
  : index(index)
{
}

SelectAssetCommand::SelectAssetCommand(const nlohmann::json& json)
  : index(json.at("index").get<int>())
{
}

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
           "Purpose: Sets the active selected asset index in the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SelectAsset\",\n"
           "  \"version\": 1,\n"
           "  \"index\": <int>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SelectAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"IndexOutOfBounds\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json SelectAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    if (index < 0 || static_cast<size_t>(index) >= database.getProject().getAssetCount())
    {
        return {
            {    "type",          getType() },
            { "version",       getVersion() },
            {  "status", "IndexOutOfBounds" },
            {   "index",              index }
        };
    }

    database.getProject().setSelectedIndex(index);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",        index }
    };
}

int SelectAssetCommand::getIndex() const
{
    return index;
}

void SelectAssetCommand::setIndex(int newIndex)
{
    index = newIndex;
}
