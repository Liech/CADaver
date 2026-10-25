#include "SaveVoxelAssetCommand.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Database.h"
#include "Project/VoxelAsset.h"

SaveVoxelAssetCommand::SaveVoxelAssetCommand() {}

SaveVoxelAssetCommand::SaveVoxelAssetCommand(int assetIndex)
  : assetIndex(assetIndex)
{
}

SaveVoxelAssetCommand::SaveVoxelAssetCommand(const nlohmann::json& json)
  : assetIndex(json.at("assetIndex").get<int>())
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
           "  \"assetIndex\": <int>\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: SVO binary stream\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"NoOutputStream\" | \"AssetNotFound\" | \"NotVoxelAsset\",\n"
           "  \"index\": <int>\n"
           "}";
}

nlohmann::json SaveVoxelAssetCommand::execute(Database& database, std::istream* inputStream, std::ostream* outputStream)
{
    if (!outputStream)
    {
        return {
            {    "type",        getType() },
            { "version",     getVersion() },
            {  "status", "NoOutputStream" },
            {   "index",       assetIndex }
        };
    }

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

    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(baseAsset);
    if (!voxelAsset || !voxelAsset->getSVO())
    {
        return {
            {    "type",       getType() },
            { "version",    getVersion() },
            {  "status", "NotVoxelAsset" },
            {   "index",      assetIndex }
        };
    }

    CADacomb::SVOFile::save(*outputStream, *voxelAsset->getSVO(), voxelAsset->getAABB());
    voxelAsset->markClean();

    return {
        {    "type",    getType() },
        { "version", getVersion() },
        {  "status",    "Success" },
        {   "index",   assetIndex }
    };
}

int SaveVoxelAssetCommand::getAssetIndex() const
{
    return assetIndex;
}

void SaveVoxelAssetCommand::setAssetIndex(int index)
{
    assetIndex = index;
}
