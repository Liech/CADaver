#include "Commands/IO/SaveVoxelAssetCommand.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

SaveVoxelAssetCommand::SaveVoxelAssetCommand() {}

SaveVoxelAssetCommand::SaveVoxelAssetCommand(const CADacomb::GUID& assetGuid)
  : assetGuid(assetGuid)
{
}

SaveVoxelAssetCommand::SaveVoxelAssetCommand(const nlohmann::json& json)
  : assetGuid(json.value("assetGuid", ""))
{
}

SaveVoxelAssetCommand::~SaveVoxelAssetCommand() {}

std::string SaveVoxelAssetCommand::getType() const
{
    return "SaveVoxelAsset";
}

int SaveVoxelAssetCommand::getVersion() const
{
    return 1;
}

std::string SaveVoxelAssetCommand::getDescription() const
{
    return "Command: SaveVoxelAsset\n"
           "Version: 1\n"
           "Purpose: Saves an SVO voxel asset to an output stream.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: SVO binary data\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotVoxelAsset\" | \"NoSVO\" | \"NoOutputStream\"\n"
           "}";
}

nlohmann::json SaveVoxelAssetCommand::execute(Database& database, std::istream*, std::ostream* outputStream, bool)
{
    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "AssetNotFound" }
        };
    }

    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(baseAsset);
    if (!voxelAsset)
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "NotVoxelAsset" }
        };
    }

    if (!voxelAsset->getSVO())
    {
        return {
            {    "type",    getType() },
            { "version", getVersion() },
            {  "status",      "NoSVO" }
        };
    }

    if (!outputStream)
    {
        return {
            {    "type",        getType() },
            { "version",     getVersion() },
            {  "status", "NoOutputStream" }
        };
    }

    CADacomb::SVOFile::save(*outputStream, *voxelAsset->getSVO(), voxelAsset->getAABB());

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" }
    };
}

const CADacomb::GUID& SaveVoxelAssetCommand::getAssetGuid() const
{
    return assetGuid;
}

void SaveVoxelAssetCommand::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}
