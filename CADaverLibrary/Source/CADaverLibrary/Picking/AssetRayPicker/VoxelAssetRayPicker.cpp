#include "Picking/AssetRayPicker/VoxelAssetRayPicker.h"

#include "Database/Asset/VoxelAsset.h"
#include "Picking/AssetPicker.h"

#include <limits>

VoxelAssetRayPicker::VoxelAssetRayPicker() {}

VoxelAssetRayPicker::~VoxelAssetRayPicker() {}

CADacomb::AABB3 VoxelAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto& voxelAsset = static_cast<const VoxelAsset&>(asset);
    return voxelAsset.getAABB();
}

PickResult VoxelAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    PickResult  result;
    const auto& voxelAsset = static_cast<const VoxelAsset&>(asset);
    const auto& svo        = voxelAsset.getSVO();
    const auto& aabb       = voxelAsset.getAABB();

    double entryDist = 0.0;
    if (!AssetPicker::intersectAABB(ray, aabb, entryDist))
    {
        return result;
    }

    const int        dimension  = static_cast<int>(svo.getDimension());
    const glm::dvec3 voxelScale = aabb.getSize() / static_cast<double>(dimension);

    double closestDistance = std::numeric_limits<double>::max();
    bool   hasHit          = false;

    svo.traverse(
      [&](const CADacomb::SVONode<bool>& node, const glm::ivec3& nodeOffset, uint32_t nodeDim, int depth, bool isLeaf) -> bool
      {
          CADacomb::AABB3 nodeAABB(aabb.getMin() + glm::dvec3(nodeOffset) * voxelScale, aabb.getMin() + glm::dvec3(nodeOffset + glm::ivec3(nodeDim)) * voxelScale);

          double nodeDist = 0.0;
          if (!AssetPicker::intersectAABB(ray, nodeAABB, nodeDist))
          {
              return false;
          }

          if (nodeDist >= closestDistance)
          {
              return false;
          }

          if (isLeaf)
          {
              if (node.value)
              {
                  closestDistance = nodeDist;
                  hasHit          = true;
              }
              return false;
          }

          return true;
      });

    if (hasHit)
    {
        result.setHit(asset.getGuid(), asset.getType(), closestDistance, ray.getOrigin() + ray.getDirection() * closestDistance);
    }

    return result;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VoxelAssetRayPicker/HitsSolidVoxelNotEmptyAABB")
{
    VoxelAsset asset;
    auto       svo = std::make_unique<CADacomb::SVO<bool>>(8, false);
    svo->setVoxel(glm::ivec3(6, 4, 4), true);
    asset.setSVO(std::move(svo));
    asset.setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(8.0)));

    VoxelAssetRayPicker picker;

    PickResult hit = picker.pick(CADacomb::Ray(glm::dvec3(-2.0, 4.5, 4.5), glm::dvec3(1.0, 0.0, 0.0)), asset);
    REQUIRE(hit.isHit());
    CHECK(hit.getDistance() == Catch::Approx(8.0).margin(1e-6));
    CHECK(hit.getHitPoint().x == Catch::Approx(6.0).margin(1e-6));

    PickResult miss = picker.pick(CADacomb::Ray(glm::dvec3(-2.0, 1.5, 1.5), glm::dvec3(1.0, 0.0, 0.0)), asset);
    CHECK_FALSE(miss.isHit());

    PickResult fromAbove = picker.pick(CADacomb::Ray(glm::dvec3(6.5, 20.0, 4.5), glm::dvec3(0.0, -1.0, 0.0)), asset);
    REQUIRE(fromAbove.isHit());
    CHECK(fromAbove.getHitPoint().y == Catch::Approx(5.0).margin(1e-6));
}

TEST_CASE("VoxelAssetRayPicker/PicksClosestOfMultipleVoxels")
{
    VoxelAsset asset;
    auto       svo = std::make_unique<CADacomb::SVO<bool>>(8, false);
    svo->setVoxel(glm::ivec3(2, 4, 4), true);
    svo->setVoxel(glm::ivec3(6, 4, 4), true);
    asset.setSVO(std::move(svo));
    asset.setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(8.0)));

    VoxelAssetRayPicker picker;

    PickResult hitFromLeft = picker.pick(CADacomb::Ray(glm::dvec3(-2.0, 4.5, 4.5), glm::dvec3(1.0, 0.0, 0.0)), asset);
    REQUIRE(hitFromLeft.isHit());
    CHECK(hitFromLeft.getDistance() == Catch::Approx(4.0).margin(1e-6));
    CHECK(hitFromLeft.getHitPoint().x == Catch::Approx(2.0).margin(1e-6));

    PickResult hitFromRight = picker.pick(CADacomb::Ray(glm::dvec3(10.0, 4.5, 4.5), glm::dvec3(-1.0, 0.0, 0.0)), asset);
    REQUIRE(hitFromRight.isHit());
    CHECK(hitFromRight.getDistance() == Catch::Approx(3.0).margin(1e-6));
    CHECK(hitFromRight.getHitPoint().x == Catch::Approx(7.0).margin(1e-6));
}

TEST_CASE("VoxelAssetRayPicker/EmptySVO")
{
    VoxelAsset asset;
    auto       svo = std::make_unique<CADacomb::SVO<bool>>(8, false);
    asset.setSVO(std::move(svo));
    asset.setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(8.0)));

    VoxelAssetRayPicker picker;
    PickResult          hit = picker.pick(CADacomb::Ray(glm::dvec3(-2.0, 4.5, 4.5), glm::dvec3(1.0, 0.0, 0.0)), asset);
    CHECK_FALSE(hit.isHit());
}
#endif
