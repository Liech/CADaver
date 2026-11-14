#pragma once

#include "CADacomb/Util/GUID.h"

#include <cstddef>
#include <glm/glm.hpp>
#include <string>

// Result of a 3D viewport pick query against project assets.
class PickResult
{
  public:
    PickResult();
    virtual ~PickResult();

    bool                  isHit() const;
    const CADacomb::GUID& getAssetGuid() const;
    const std::string&    getAssetType() const;
    double                getDistance() const;
    const glm::dvec3&     getHitPoint() const;
    bool                  hasTriangle() const;
    size_t                getTriangleIndex() const;

    void setHit(const CADacomb::GUID& guid, const std::string& type, double dist, const glm::dvec3& point);
    void setTriangleIndex(size_t index);

  private:
    bool           hit              = false;
    CADacomb::GUID assetGuid;
    std::string    assetType;
    double         distance         = 0.0;
    glm::dvec3     hitPoint         = glm::dvec3(0.0);
    bool           hasTriangleIndex = false;
    size_t         triangleIndex    = 0;
};
