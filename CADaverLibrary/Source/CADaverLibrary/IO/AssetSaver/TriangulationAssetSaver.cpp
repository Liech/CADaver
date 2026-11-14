#include "IO/AssetSaver/TriangulationAssetSaver.h"

#include "CADacomb/Util/ProgressBar.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>
#include <vector>

TriangulationAssetSaver::TriangulationAssetSaver() {}

TriangulationAssetSaver::~TriangulationAssetSaver() {}

bool TriangulationAssetSaver::save(const Asset& asset, const Polyglot::Node& destination) const
{
    CADacomb::ProgressBar bar(2, "Saving STL asset");
    if (bar.isCanceled())
    {
        return false;
    }

    const auto&        triAsset = static_cast<const TriangulationAsset&>(asset);
    std::ostringstream memStream(std::ios::binary);
    triAsset.getTriangulation().saveAsSTL(memStream);
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
