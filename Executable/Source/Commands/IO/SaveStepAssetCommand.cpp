#include "Commands/IO/SaveStepAssetCommand.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Database.h"
#include "Library/CAD/CADShape.h"

SaveStepAssetCommand::SaveStepAssetCommand() {}

SaveStepAssetCommand::SaveStepAssetCommand(const CADacomb::GUID& assetGuid, const std::string& filePath)
  : filePath(filePath)
  , assetGuid(assetGuid)
{
}

SaveStepAssetCommand::SaveStepAssetCommand(const nlohmann::json& json)
  : filePath(json.value("filePath", ""))
  , assetGuid(json.value("assetGuid", ""))
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
           "Purpose: Saves a CAD asset to a STEP file.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"filePath\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: None\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveStepAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotCadAsset\" | \"NoShape\" | \"WriteFailed\",\n"
           "  \"filePath\": <string>\n"
           "}";
}

nlohmann::json SaveStepAssetCommand::execute(Database& database, std::istream*, std::ostream*, bool)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return {
            {     "type",       getType() },
            {  "version",    getVersion() },
            {   "status", "AssetNotFound" },
            { "filePath",        filePath }
        };
    }

    auto cadAsset = std::dynamic_pointer_cast<CadAsset>(baseAsset);
    if (!cadAsset)
    {
        return {
            {     "type",     getType() },
            {  "version",  getVersion() },
            {   "status", "NotCadAsset" },
            { "filePath",      filePath }
        };
    }

    auto* shape = cadAsset->getCadShape();
    if (!shape)
    {
        return {
            {     "type",    getType() },
            {  "version", getVersion() },
            {   "status",    "NoShape" },
            { "filePath",     filePath }
        };
    }

    std::string targetPath = filePath.empty() ? cadAsset->getFilePath() : filePath;
    if (targetPath.empty())
    {
        targetPath = cadAsset->getName() + ".step";
    }

    if (!shape->save(targetPath))
    {
        return {
            {     "type",     getType() },
            {  "version",  getVersion() },
            {   "status", "WriteFailed" },
            { "filePath",    targetPath }
        };
    }

    return {
        {     "type",    getType() },
        {  "version", getVersion() },
        {   "status",    "Success" },
        { "filePath",   targetPath }
    };
}

const CADacomb::GUID& SaveStepAssetCommand::getAssetGuid() const
{
    return assetGuid;
}

void SaveStepAssetCommand::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}

const std::string& SaveStepAssetCommand::getFilePath() const
{
    return filePath;
}

void SaveStepAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}
