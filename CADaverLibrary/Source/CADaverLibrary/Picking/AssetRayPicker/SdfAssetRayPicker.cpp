#include "Picking/AssetRayPicker/SdfAssetRayPicker.h"

#include "CADacomb/Formula/FormulaLuaEvaluator.h"
#include "Database/Asset/SdfAsset.h"

#include <algorithm>
#include <cmath>
#include <limits>

SdfAssetRayPicker::SdfAssetRayPicker() {}

SdfAssetRayPicker::~SdfAssetRayPicker() {}

CADacomb::AABB3 SdfAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto& sdfAsset = static_cast<const SdfAsset&>(asset);
    return sdfAsset.getAABB();
}

bool SdfAssetRayPicker::intersectAABBInterval(const CADacomb::Ray& ray, const CADacomb::AABB3& aabb, double& outNear, double& outFar)
{
    const glm::dvec3& origin = ray.getOrigin();
    const glm::dvec3& dir    = ray.getDirection();
    const glm::dvec3& boxMin = aabb.getMin();
    const glm::dvec3& boxMax = aabb.getMax();

    double tNear = -std::numeric_limits<double>::infinity();
    double tFar  = std::numeric_limits<double>::infinity();

    for (int axis = 0; axis < 3; axis++)
    {
        if (std::abs(dir[axis]) < 1e-12)
        {
            if (origin[axis] < boxMin[axis] || origin[axis] > boxMax[axis])
            {
                return false;
            }
        }
        else
        {
            double invD = 1.0 / dir[axis];
            double t0   = (boxMin[axis] - origin[axis]) * invD;
            double t1   = (boxMax[axis] - origin[axis]) * invD;
            if (t0 > t1)
            {
                std::swap(t0, t1);
            }
            tNear = std::max(tNear, t0);
            tFar  = std::min(tFar, t1);
            if (tNear > tFar)
            {
                return false;
            }
        }
    }

    if (tFar < 0.0)
    {
        return false;
    }

    outNear = std::max(0.0, tNear);
    outFar  = tFar;
    return true;
}

PickResult SdfAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    PickResult  result;
    const auto& sdfAsset = static_cast<const SdfAsset&>(asset);
    const auto& aabb     = sdfAsset.getAABB();

    double tNear = 0.0;
    double tFar  = 0.0;
    if (!intersectAABBInterval(ray, aabb, tNear, tFar))
    {
        return result;
    }

    CADacomb::FormulaLuaEvaluator evaluator;
    std::string                   error;
    if (!evaluator.compile(sdfAsset.getFormula(), error))
    {
        return result;
    }

    const int    maxSteps   = 128;
    const double hitEpsilon = 1e-4;
    double       t          = tNear;
    glm::dvec3   origin     = ray.getOrigin();
    glm::dvec3   direction  = ray.getDirection();

    for (int step = 0; step < maxSteps && t <= tFar; step++)
    {
        glm::dvec3         p      = origin + direction * t;
        std::vector<float> output = evaluator.evaluate({ static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) });
        if (output.empty())
        {
            return result;
        }

        double dist = static_cast<double>(output[0]);
        if (dist <= hitEpsilon)
        {
            result.setHit(asset.getGuid(), asset.getType(), t, p);
            return result;
        }

        t += dist;
    }

    return result;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SdfAssetRayPicker/HitsSphereAndMissesOutside")
{
    SdfAsset asset;
    asset.setAABB(CADacomb::AABB3(glm::dvec3(-1.0), glm::dvec3(1.0)));

    SdfAssetRayPicker picker;

    PickResult hit = picker.pick(CADacomb::Ray(glm::dvec3(-3.0, 0.0, 0.0), glm::dvec3(1.0, 0.0, 0.0)), asset);
    REQUIRE(hit.isHit());
    CHECK(hit.getDistance() == Catch::Approx(2.5).margin(1e-3));
    CHECK(hit.getHitPoint().x == Catch::Approx(-0.5).margin(1e-3));

    PickResult miss = picker.pick(CADacomb::Ray(glm::dvec3(-3.0, 0.8, 0.0), glm::dvec3(1.0, 0.0, 0.0)), asset);
    CHECK_FALSE(miss.isHit());
}
#endif
