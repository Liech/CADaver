#include "RemoveAssetCommand.h"

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

nlohmann::json RemoveAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
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

    database.getProject().removeAsset(static_cast<size_t>(assetIndex));

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
