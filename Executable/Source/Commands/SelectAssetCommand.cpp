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

nlohmann::json SelectAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    database.getProject().setSelectedIndex(index);
    return {
        {    "type",                                getType() },
        { "version",                             getVersion() },
        {  "status",                                "Success" },
        {   "index", database.getProject().getSelectedIndex() }
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
