#include "Commands/IO/SaveVoxelAssetCommand.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Database.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>

SaveVoxelAssetCommand::SaveVoxelAssetCommand() {}

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
           "Purpose: Saves an SVO voxel asset to a destination callback or file path.\n\n"
           "Input Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"assetGuid\": <string>,\n"
           "  \"destination\": <callback or string>\n"
           "}\n\n"
           "Output Syntax (JSON):\n"
           "{\n"
           "  \"type\": \"SaveVoxelAsset\",\n"
           "  \"version\": 1,\n"
           "  \"status\": \"Success\" | \"AssetNotFound\" | \"NotVoxelAsset\" | \"NoSVO\"\n"
           "}";
}

std::vector<CADacomb::GUID> SaveVoxelAssetCommand::getRequiredAssets() const
{
    return { assetGuid };
}

void SaveVoxelAssetCommand::setInput(const Polyglot::Node& node)
{
    auto& mutableNode = const_cast<Polyglot::Node&>(node);
    assetGuid         = static_cast<std::string>(mutableNode["assetGuid"]);
    destination       = mutableNode["destination"];
}

nlohmann::json SaveVoxelAssetCommand::execute(Database& database)
{
    CADacomb::ProgressBar bar(2, "Saving SVO asset");

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

    std::ostringstream memStream(std::ios::binary);
    CADacomb::SVOFile::save(memStream, voxelAsset->getSVO(), voxelAsset->getAABB());
    bar.addProgress(1);
    if (bar.isCanceled())
    {
        return createResponse("WriteFailed");
    }

    std::string          str = memStream.str();
    std::vector<uint8_t> bytes(str.begin(), str.end());

    if (!IOValueGrabber::deliverBytes(destination, bytes))
    {
        return createResponse("WriteFailed");
    }

    bar.addProgress(1);

    return createResponse("Success");
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveVoxelAssetCommand/TypeAndVersion")
{
    CADacomb::GUID        guid = CADacomb::GUID::random();
    SaveVoxelAssetCommand cmd;
    cmd.setInput(Polyglot::Node({
      {   "assetGuid",      guid.toString() },
      { "destination", "output.cadacombsvo" }
    }));
    CHECK(cmd.getType() == "SaveVoxelAsset");
    CHECK(cmd.getVersion() == 1);
    CHECK_FALSE(cmd.getDescription().empty());
    REQUIRE(cmd.getRequiredAssets().size() == 1);
    CHECK(cmd.getRequiredAssets().front() == guid);
}
#endif
