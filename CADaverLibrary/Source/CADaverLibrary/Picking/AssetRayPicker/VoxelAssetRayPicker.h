#pragma once

#include "Picking/AssetRayPicker/AssetRayPicker.h"

// Ray picker for VoxelAsset.
class VoxelAssetRayPicker : public AssetRayPicker
{
  public:
    VoxelAssetRayPicker();
    virtual ~VoxelAssetRayPicker();

    virtual CADacomb::AABB3 getAABB(const Asset& asset) const override;
    virtual PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const override;
};
