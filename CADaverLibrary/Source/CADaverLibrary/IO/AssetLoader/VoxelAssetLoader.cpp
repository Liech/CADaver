#include "IO/AssetLoader/VoxelAssetLoader.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Utility/IOValueGrabber.h"

#include <sstream>
#include <vector>

VoxelAssetLoader::VoxelAssetLoader() {}

VoxelAssetLoader::~VoxelAssetLoader() {}

std::shared_ptr<Asset> VoxelAssetLoader::load(const std::string& name, const Polyglot::Node& source) const
{
    CADacomb::ProgressBar bar(3, "Loading SVO asset");

    CADacomb::AABB3                      bounds;
    std::unique_ptr<CADacomb::SVO<bool>> svo;

    std::vector<uint8_t> bytes = IOValueGrabber::grabBytes(source);
    bar.addProgress(1);

    if (!bytes.empty())
    {
        std::string        str(bytes.begin(), bytes.end());
        std::istringstream stream(str, std::ios::binary);
        svo = CADacomb::SVOFile::load<bool>(stream, bounds);
    }
    bar.addProgress(1);

    if (!svo || bar.isCanceled())
    {
        return nullptr;
    }

    if (bounds.getSize().x <= 0.0 || bounds.getSize().y <= 0.0 || bounds.getSize().z <= 0.0)
    {
        bounds = CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(static_cast<double>(svo->getDimension())));
    }

    auto asset = std::make_shared<VoxelAsset>();
    asset->setName(name);
    asset->setSVO(std::move(svo));
    asset->setAABB(bounds);

    bar.addProgress(1);
    return asset;
}
