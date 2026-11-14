#include "Picking/AssetPicker.h"

#include "Database/Asset.h"
#include "Database/Project.h"
#include "Picking/AssetRayPicker/AssetRayPicker.h"

#include <algorithm>
#include <limits>

AssetPicker::AssetPicker() {}

AssetPicker::~AssetPicker() {}

PickResult AssetPicker::pickRay(const CADacomb::Ray& ray, const Project& project) const
{
    PickResult bestResult;
    double     closestDistance = std::numeric_limits<double>::max();

    for (const auto& asset : project.getAssets())
    {
        if (!asset)
        {
            continue;
        }

        auto typePicker = factory.create(asset->getType());
        if (!typePicker)
        {
            continue;
        }

        CADacomb::AABB3 aabb     = typePicker->getAABB(*asset);
        double          aabbDist = 0.0;
        if (!intersectAABB(ray, aabb, aabbDist) || aabbDist >= closestDistance)
        {
            continue;
        }

        PickResult candidate = typePicker->pick(ray, *asset);
        if (candidate.isHit() && candidate.getDistance() < closestDistance)
        {
            closestDistance = candidate.getDistance();
            bestResult      = candidate;
        }
    }

    return bestResult;
}

bool AssetPicker::intersectAABB(const CADacomb::Ray& ray, const CADacomb::AABB3& aabb, double& outDistance)
{
    double tMin = 0.0;
    double tMax = std::numeric_limits<double>::max();

    const glm::dvec3& origin    = ray.getOrigin();
    const glm::dvec3& direction = ray.getDirection();
    const glm::dvec3& minCorner = aabb.getMin();
    const glm::dvec3& maxCorner = aabb.getMax();

    for (int axis = 0; axis < 3; axis++)
    {
        if (glm::abs(direction[axis]) < 1e-12)
        {
            if (origin[axis] < minCorner[axis] || origin[axis] > maxCorner[axis])
            {
                return false;
            }
        }
        else
        {
            double invD = 1.0 / direction[axis];
            double t0   = (minCorner[axis] - origin[axis]) * invD;
            double t1   = (maxCorner[axis] - origin[axis]) * invD;
            if (t0 > t1)
            {
                std::swap(t0, t1);
            }
            tMin = std::max(tMin, t0);
            tMax = std::min(tMax, t1);
            if (tMax < tMin)
            {
                return false;
            }
        }
    }

    outDistance = tMin;
    return true;
}
