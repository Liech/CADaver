#include "IO/AssetSaver/VoxelAssetSaver.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>
#include <vector>

VoxelAssetSaver::VoxelAssetSaver() {}

VoxelAssetSaver::~VoxelAssetSaver() {}

bool VoxelAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    CADacomb::ProgressBar bar(2, "Saving SVO asset");
    if (bar.isCanceled())
    {
        return false;
    }

    const auto&        voxelAsset = static_cast<const VoxelAsset&>(asset);
    std::ostringstream memStream(std::ios::binary);
    CADacomb::SVOFile::save(memStream, voxelAsset.getSVO(), voxelAsset.getAABB());
    bar.addProgress(1);
    if (bar.isCanceled())
    {
        return false;
    }

    std::string          str = memStream.str();
    std::vector<uint8_t> bytes(str.begin(), str.end());
    if (!IOValueGrabber::deliverBytes(destination, bytes))
    {
        return false;
    }

    bar.addProgress(1);
    return true;
}
