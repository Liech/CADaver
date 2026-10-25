#include "LoadStepAssetCommand.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"

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

std::string LoadStepAssetCommand::getDescription() const
{
    return "Command: LoadStepAsset\n"
           "Version: 1\n"
           "Purpose: Loads a STEP CAD asset from a file path into the project.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"name\": \"<string>\",\n"
           "  \"filePath\": \"<string>\"\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"LoadStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\",\n"
           "  \"index\": <int>,\n"
           "  \"name\": \"<string>\"\n"
           "}";
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
