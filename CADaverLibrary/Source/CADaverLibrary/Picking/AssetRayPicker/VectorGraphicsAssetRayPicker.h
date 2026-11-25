#pragma once

#include "Picking/AssetRayPicker/AssetRayPicker.h"

// Ray picker and AABB provider for VectorGraphicsAsset instances on their local XY plane.
class VectorGraphicsAssetRayPicker : public AssetRayPicker
{
  public:
    VectorGraphicsAssetRayPicker();
    virtual ~VectorGraphicsAssetRayPicker();

    CADacomb::AABB3 getAABB(const Asset& asset) const override;
    PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const override;
};
