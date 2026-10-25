#include "SaveStepAssetCommand.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"
#include "Library/CAD/CADShape.h"
#include "Library/Operation/IO/SaveCADOperation.h"

SaveStepAssetCommand::SaveStepAssetCommand() {}

SaveStepAssetCommand::SaveStepAssetCommand(int assetIndex, const std::string& filePath)
  : filePath(filePath)
  , assetIndex(assetIndex)
{
}

SaveStepAssetCommand::SaveStepAssetCommand(const nlohmann::json& json)
  : filePath(json.at("filePath").get<std::string>())
  , assetIndex(json.at("assetIndex").get<int>())
{
}

SaveStepAssetCommand::~SaveStepAssetCommand() {}

std::string SaveStepAssetCommand::getType() const
{
    return "SaveStepAsset";
}

int SaveStepAssetCommand::getVersion() const
{
    return 1;
}

std::string SaveStepAssetCommand::getDescription() const
{
    return "Command: SaveStepAsset\n"
           "Version: 1\n"
           "Purpose: Saves a CAD asset to a STEP file on disk.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetIndex\": <int>,\n"
           "  \"filePath\": \"<string>\"\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json SaveStepAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    auto baseAsset = database.getProject().getAsset(static_cast<size_t>(assetIndex));
    if (!baseAsset)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "AssetNotFound" },
            {   "index",      assetIndex }
        };
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset || !cadAsset->getCadShape())
    {
        return {
            {    "type",     getType() },
            { "version",  getVersion() },
            {  "status", "NotCadAsset" },
            {   "index",    assetIndex }
        };
    }

    Library::SaveCADOperation::saveToFile(*cadAsset->getCadShape(), filePath);
    cadAsset->setFilePath(filePath);
    cadAsset->markClean();

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",   assetIndex }
    };
}

int SaveStepAssetCommand::getAssetIndex() const
{
    return assetIndex;
}

void SaveStepAssetCommand::setAssetIndex(int index)
{
    assetIndex = index;
}

const std::string& SaveStepAssetCommand::getFilePath() const
{
    return filePath;
}

void SaveStepAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}
