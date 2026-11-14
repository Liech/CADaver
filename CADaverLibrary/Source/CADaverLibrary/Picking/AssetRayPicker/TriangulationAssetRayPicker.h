#pragma once

#include "Picking/AssetRayPicker/AssetRayPicker.h"
#include "Picking/AssetRayPicker/TriangulationRayPicker.h"

// Ray picker for TriangulationAsset.
class TriangulationAssetRayPicker : public AssetRayPicker
{
  public:
    TriangulationAssetRayPicker();
    virtual ~TriangulationAssetRayPicker();

    virtual CADacomb::AABB3 getAABB(const Asset& asset) const override;
    virtual PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const override;

  private:
    TriangulationRayPicker triangulationPicker;
};
