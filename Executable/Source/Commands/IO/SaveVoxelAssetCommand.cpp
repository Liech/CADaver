#include "Commands/IO/SaveVoxelAssetCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"

#include <fstream>

SaveVoxelAssetCommand::SaveVoxelAssetCommand() {}

SaveVoxelAssetCommand::SaveVoxelAssetCommand(const CADacomb::GUID& assetGuid, const std::string& filePath)
  : filePath(filePath)
  , assetGuid(assetGuid)
{
}

SaveVoxelAssetCommand::SaveVoxelAssetCommand(const nlohmann::json& json)
  : filePath(json.value("filePath", ""))
  , assetGuid(json.value("assetGuid", ""))
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
           "Purpose: Saves an SVO voxel asset to an output stream or file path.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"filePath\": <string> (optional)\n"
           "}\n\n"
           "Input Stream: None\n"
           "Output Stream: SVO binary data (optional if filePath specified)\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotVoxelAsset\" | \"NoSVO\" | \"NoOutputStream\"\n"
           "}";
}

std::vector<CADacomb::GUID> SaveVoxelAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

nlohmann::json SaveVoxelAssetCommand::execute(Database& database, std::istream*, std::ostream* outputStream, bool)
{
    CADacomb::ProgressBar bar(1, "Saving SVO asset");

    auto baseAsset = database.getProject().getAsset(assetGuid);
    if (!baseAsset || bar.isCanceled())
    {
        return createResponse("AssetNotFound");
    }

    auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(baseAsset);
    if (!voxelAsset)
    {
        return createResponse("NotVoxelAsset");
    }

    if (!voxelAsset->getSVO())
    {
        return createResponse("NoSVO");
    }

    std::ofstream fileStream;
    std::ostream* streamToUse = outputStream;
    if (!streamToUse && !filePath.empty())
    {
        fileStream.open(filePath, std::ios::binary);
        if (fileStream.is_open())
        {
            streamToUse = &fileStream;
        }
    }

    if (!streamToUse || bar.isCanceled())
    {
        return createResponse("NoOutputStream");
    }

    CADacomb::SVOFile::save(*streamToUse, *voxelAsset->getSVO(), voxelAsset->getAABB());
    bar.addProgress(1);

    return createResponse("Success");
}

const CADacomb::GUID& SaveVoxelAssetCommand::getAssetGuid() const
{
    return assetGuid;
}

void SaveVoxelAssetCommand::setAssetGuid(const CADacomb::GUID& guid)
{
    assetGuid = guid;
}

const std::string& SaveVoxelAssetCommand::getFilePath() const
{
    return filePath;
}

void SaveVoxelAssetCommand::setFilePath(const std::string& newFilePath)
{
    filePath = newFilePath;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveVoxelAssetCommand/TypeAndVersion")
{
    CADacomb::GUID        guid = CADacomb::GUID::random();
    SaveVoxelAssetCommand cmd(guid, "output.cadacombsvo");
    CHECK(cmd.getType() == "SaveVoxelAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    CHECK(cmd.getAssetGuid() == guid);
    CHECK(cmd.getFilePath() == "output.cadacombsvo");
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
