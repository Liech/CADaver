#include "Picking/AssetPicker.h"

#include "Database/Asset.h"
#include "Database/Project.h"
#include "Picking/AssetRayPicker/AssetRayPicker.h"

#include <glm/gtc/matrix_inverse.hpp>

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

        const glm::dmat4& model = asset->getTransformation();
        glm::dmat4        inverseModel = glm::inverse(model);
        glm::dvec3        localOrigin  = glm::dvec3(inverseModel * glm::dvec4(ray.getOrigin(), 1.0));
        glm::dvec3        localDir     = glm::dvec3(inverseModel * glm::dvec4(ray.getDirection(), 0.0));
        CADacomb::Ray     localRay(localOrigin, localDir);

        CADacomb::AABB3 aabb     = typePicker->getAABB(*asset);
        double          aabbDist = 0.0;
        if (!intersectAABB(localRay, aabb, aabbDist))
        {
            continue;
        }

        glm::dvec3 worldEntry = glm::dvec3(model * glm::dvec4(localRay.getOrigin() + localRay.getDirection() * aabbDist, 1.0));
        if (glm::length(worldEntry - ray.getOrigin()) >= closestDistance)
        {
            continue;
        }

        PickResult candidate = typePicker->pick(localRay, *asset);
        if (!candidate.isHit())
        {
            continue;
        }

        glm::dvec3 worldHit      = glm::dvec3(model * glm::dvec4(candidate.getHitPoint(), 1.0));
        double     worldDistance = glm::length(worldHit - ray.getOrigin());
        if (worldDistance < closestDistance)
        {
            candidate.setHit(candidate.getAssetGuid(), candidate.getAssetType(), worldDistance, worldHit);
            closestDistance = worldDistance;
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

#ifdef ISTESTPROJECT
#include "Database/Asset/VoxelAsset.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/gtc/matrix_transform.hpp>

TEST_CASE("AssetPicker/PicksInTransformedAssetSpace")
{
    auto asset = std::make_shared<VoxelAsset>();
    auto svo   = std::make_unique<CADacomb::SVO<bool>>(8, false);
    svo->setVoxel(glm::ivec3(6, 4, 4), true);
    asset->setSVO(std::move(svo));
    asset->setAABB(CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(8.0)));
    asset->setTransformation(glm::translate(glm::dmat4(1.0), glm::dvec3(10.0, 0.0, 0.0)) * glm::rotate(glm::dmat4(1.0), glm::radians(90.0), glm::dvec3(0.0, 0.0, 1.0)));

    Project project;
    project.addAsset(asset);
    AssetPicker picker;

    // Rotated 90 degrees about Z: local (6.5, 4.5, 4.5) maps to world (10 - 4.5, 6.5, 4.5).
    PickResult hit = picker.pickRay(CADacomb::Ray(glm::dvec3(5.5, 20.0, 4.5), glm::dvec3(0.0, -1.0, 0.0)), project);
    REQUIRE(hit.isHit());
    CHECK(hit.getDistance() == Catch::Approx(13.0).margin(1e-6));
    CHECK(hit.getHitPoint().y == Catch::Approx(7.0).margin(1e-6));

    PickResult miss = picker.pickRay(CADacomb::Ray(glm::dvec3(6.5, 20.0, 4.5), glm::dvec3(0.0, -1.0, 0.0)), project);
    CHECK_FALSE(miss.isHit());
}
#endif