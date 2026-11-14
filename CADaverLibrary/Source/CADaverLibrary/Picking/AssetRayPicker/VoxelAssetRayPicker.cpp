#include "Picking/AssetRayPicker/VoxelAssetRayPicker.h"

#include "Database/Asset/VoxelAsset.h"
#include "Picking/AssetPicker.h"

VoxelAssetRayPicker::VoxelAssetRayPicker() {}

VoxelAssetRayPicker::~VoxelAssetRayPicker() {}

CADacomb::AABB3 VoxelAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto& voxelAsset = static_cast<const VoxelAsset&>(asset);
    return voxelAsset.getAABB();
}

PickResult VoxelAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    PickResult result;
    double     hitDist = 0.0;
    if (AssetPicker::intersectAABB(ray, getAABB(asset), hitDist))
    {
        result.setHit(asset.getGuid(), asset.getType(), hitDist, ray.getOrigin() + ray.getDirection() * hitDist);
    }

    return result;
}
