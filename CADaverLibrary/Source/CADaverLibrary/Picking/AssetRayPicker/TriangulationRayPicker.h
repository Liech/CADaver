#pragma once

#include "CADacomb/Util/AABB3.h"
#include "CADacomb/Util/GUID.h"
#include "CADacomb/Util/Ray.h"
#include "Picking/PickResult.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <string>

namespace CADacomb
{
    class Triangulation;
}

// Performs ray intersection queries against a CADacomb::Triangulation.
class TriangulationRayPicker
{
  public:
    TriangulationRayPicker();
    virtual ~TriangulationRayPicker();

    CADacomb::AABB3 getAABB(const CADacomb::Triangulation& triangulation) const;
    PickResult      pick(const CADacomb::Ray& ray, const CADacomb::Triangulation& triangulation, const CADacomb::GUID& assetGuid, const std::string& assetType) const;

  private:
    bool intersectTriangle(const CADacomb::Ray& ray, const glm::dvec3& v0, const glm::dvec3& v1, const glm::dvec3& v2, double& outDistance) const;
};
