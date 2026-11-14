#pragma once

#include "Picking/AssetRayPicker/AssetRayPicker.h"
#include "Picking/AssetRayPicker/TriangulationRayPicker.h"

// Ray picker for CadAsset.
class CadAssetRayPicker : public AssetRayPicker
{
  public:
    CadAssetRayPicker();
    virtual ~CadAssetRayPicker();

    virtual CADacomb::AABB3 getAABB(const Asset& asset) const override;
    virtual PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const override;

  private:
    TriangulationRayPicker triangulationPicker;
};
