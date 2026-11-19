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
    std::ostringstream stream(std::ios::binary);
    CADacomb::SVOFile::save(stream, voxelAsset.getSVO(), voxelAsset.getAABB(), voxelAsset.getTransformation());
    bar.addProgress(1);
    if (bar.isCanceled())
    {
        return false;
    }

    std::string          str = stream.str();
    std::vector<uint8_t> bytes(str.begin(), str.end());
    if (!IOValueGrabber::deliverBytes(destination, bytes))
    {
        return false;
    }

    bar.addProgress(1);
    return true;
}

#ifdef ISTESTPROJECT
#include "IO/AssetLoader/VoxelAssetLoader.h"

#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("VoxelAssetSaver/RoundTripsTransformation")
{
    VoxelAsset asset;
    auto       svo = std::make_unique<CADacomb::SVO<bool>>(4, false);
    svo->setVoxel(glm::ivec3(1, 2, 3), true);
    asset.setSVO(std::move(svo));
    asset.setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(4.0)));
    glm::dmat4 targetTransform = glm::translate(glm::dmat4(1.0), glm::dvec3(3.0, 4.0, 5.0));
    asset.setTransformation(targetTransform);

    std::vector<uint8_t> savedBytes;
    Polyglot::Node       destCallback(std::function<Polyglot::Node(const Polyglot::Node&)>(
      [&savedBytes](const Polyglot::Node& data) -> Polyglot::Node
      {
          savedBytes = data.asBinary();
          return Polyglot::Node(true);
      }));

    VoxelAssetSaver saver;
    REQUIRE(saver.save(asset, destCallback));
    REQUIRE_FALSE(savedBytes.empty());

    VoxelAssetLoader loader;
    auto             loadedBase = loader.load("TestVoxel", Polyglot::Node(savedBytes));
    auto             loaded     = std::dynamic_pointer_cast<VoxelAsset>(loadedBase);
    REQUIRE(loaded != nullptr);
    CHECK(loaded->getTransformation() == targetTransform);
    CHECK(loaded->getSVO().getVoxel(glm::ivec3(1, 2, 3)));
}
#endif
