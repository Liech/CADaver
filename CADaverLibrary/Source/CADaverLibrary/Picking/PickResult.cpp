#include "Picking/PickResult.h"

PickResult::PickResult() {}

PickResult::~PickResult() {}

bool PickResult::isHit() const
{
    return hit;
}

const CADacomb::GUID& PickResult::getAssetGuid() const
{
    return assetGuid;
}

const std::string& PickResult::getAssetType() const
{
    return assetType;
}

double PickResult::getDistance() const
{
    return distance;
}

const glm::dvec3& PickResult::getHitPoint() const
{
    return hitPoint;
}

bool PickResult::hasTriangle() const
{
    return hasTriangleIndex;
}

size_t PickResult::getTriangleIndex() const
{
    return triangleIndex;
}

void PickResult::setHit(const CADacomb::GUID& guid, const std::string& type, double dist, const glm::dvec3& point)
{
    hit       = true;
    assetGuid = guid;
    assetType = type;
    distance  = dist;
    hitPoint  = point;
}

void PickResult::setTriangleIndex(size_t index)
{
    hasTriangleIndex = true;
    triangleIndex    = index;
}
