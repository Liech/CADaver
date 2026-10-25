#include "LoadStepAssetCommand.h"

#include "Database/Database.h"
#include "Project/CadAsset.h"

LoadStepAssetCommand::LoadStepAssetCommand() {}

LoadStepAssetCommand::LoadStepAssetCommand(const std::string& name)
  : name(name)
{
}

LoadStepAssetCommand::LoadStepAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
{
}

LoadStepAssetCommand::~LoadStepAssetCommand() {}

std::string LoadStepAssetCommand::getType() const
{
    return "LoadStepAsset";
}

int LoadStepAssetCommand::getVersion() const
{
    return 1;
}

nlohmann::json LoadStepAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    if (!inputStream)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "NoInputStream" },
            {   "index",              -1 },
            {    "name",            name }
        };
    }

    return {
        {    "type",           getType() },
        { "version",        getVersion() },
        {  "status", "UnsupportedStream" },
        {   "index",                  -1 },
        {    "name",                name }
    };
}

const std::string& LoadStepAssetCommand::getName() const
{
    return name;
}

void LoadStepAssetCommand::setName(const std::string& newName)
{
    name = newName;
}
