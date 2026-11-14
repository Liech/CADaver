#pragma once

#include "CADacomb/Util/AABB3.h"
#include "CADacomb/Util/Ray.h"
#include "Picking/AssetRayPicker/AssetRayPickerFactory.h"
#include "Picking/PickResult.h"

class Project;

// Performs CPU raycasting against project assets using coarse AABB culling and type-specific pickers.
class AssetPicker
{
  public:
    AssetPicker();
    virtual ~AssetPicker();

    PickResult pickRay(const CADacomb::Ray& ray, const Project& project) const;

    static bool intersectAABB(const CADacomb::Ray& ray, const CADacomb::AABB3& aabb, double& outDistance);

  private:
    AssetRayPickerFactory factory;
};
