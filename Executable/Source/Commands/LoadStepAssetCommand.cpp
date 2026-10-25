#include "LoadStepAssetCommand.h"

#include "Database/Database.h"
#include "Project/CadAsset.h"

LoadStepAssetCommand::LoadStepAssetCommand() {}

LoadStepAssetCommand::LoadStepAssetCommand(const std::string& name, const std::string& filePath)
  : name(name)
  , filePath(filePath)
{
}

LoadStepAssetCommand::LoadStepAssetCommand(const nlohmann::json& json)
  : name(json.at("name").get<std::string>())
  , filePath(json.at("filePath").get<std::string>())
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
    auto asset = std::make_shared<CadAsset>(name, filePath);
    database.getProject().addAsset(asset);
    int newIndex = static_cast<int>(database.getProject().getAssetCount()) - 1;
    database.getProject().setSelectedIndex(newIndex);

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",     newIndex },
        {    "name",         name }
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

const std::string& LoadStepAssetCommand::getFilePath() const
{
    return filePath;
}

void LoadStepAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}
