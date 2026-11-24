#pragma once

#include "Picking/AssetRayPicker/AssetRayPicker.h"

// Ray picker and local AABB provider for TextureAsset quads in the local XY plane.
class TextureAssetRayPicker : public AssetRayPicker
{
  public:
    TextureAssetRayPicker();
    virtual ~TextureAssetRayPicker();

    CADacomb::AABB3 getAABB(const Asset& asset) const override;
    PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const override;
};
