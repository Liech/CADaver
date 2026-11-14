#pragma once

#include "CADacomb/Util/AABB3.h"
#include "CADacomb/Util/Ray.h"
#include "Picking/PickResult.h"

class Asset;

// Abstract base class for type-specific asset ray picking.
class AssetRayPicker
{
  public:
    AssetRayPicker();
    virtual ~AssetRayPicker();

    virtual CADacomb::AABB3 getAABB(const Asset& asset) const                         = 0;
    virtual PickResult      pick(const CADacomb::Ray& ray, const Asset& asset) const = 0;
};
