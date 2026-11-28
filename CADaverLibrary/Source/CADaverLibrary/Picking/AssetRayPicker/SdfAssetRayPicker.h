#pragma once

#include "Picking/AssetRayPicker/AssetRayPicker.h"

// Sphere-tracing ray picker for SdfAsset.
class SdfAssetRayPicker : public AssetRayPicker
{
  public:
    SdfAssetRayPicker();
    virtual ~SdfAssetRayPicker();

    virtual CADacomb::AABB3 getAABB(const Asset& asset) const override;
    virtual PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const override;

  private:
    static bool intersectAABBInterval(const CADacomb::Ray& ray, const CADacomb::AABB3& aabb, double& outNear, double& outFar);
};
