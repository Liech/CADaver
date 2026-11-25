#include "Picking/AssetRayPicker/VectorGraphicsAssetRayPicker.h"

#include "Database/Asset/VectorGraphicsAsset.h"

#include <cmath>
#include <glm/glm.hpp>

VectorGraphicsAssetRayPicker::VectorGraphicsAssetRayPicker() {}

VectorGraphicsAssetRayPicker::~VectorGraphicsAssetRayPicker() {}

CADacomb::AABB3 VectorGraphicsAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto&       vectorAsset = static_cast<const VectorGraphicsAsset&>(asset);
    const glm::dvec2& size        = vectorAsset.getWorldSize();
    double            halfW       = size.x * 0.5;
    double            halfH       = size.y * 0.5;
    return CADacomb::AABB3(glm::dvec3(-halfW, -halfH, -0.001), glm::dvec3(halfW, halfH, 0.001));
}

PickResult VectorGraphicsAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    PickResult        result;
    const auto&       vectorAsset = static_cast<const VectorGraphicsAsset&>(asset);
    const glm::dvec2& size        = vectorAsset.getWorldSize();

    glm::dvec3 origin    = glm::dvec3(ray.getOrigin());
    glm::dvec3 direction = glm::dvec3(ray.getDirection());

    if (std::abs(direction.z) < 1e-9)
    {
        return result;
    }

    double t = -origin.z / direction.z;
    if (t < 0.0)
    {
        return result;
    }

    glm::dvec3 hitPoint = origin + direction * t;
    double     halfW    = size.x * 0.5;
    double     halfH    = size.y * 0.5;

    if (hitPoint.x >= -halfW && hitPoint.x <= halfW && hitPoint.y >= -halfH && hitPoint.y <= halfH)
    {
        result.setHit(vectorAsset.getGuid(), vectorAsset.getType(), t, hitPoint);
    }

    return result;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VectorGraphicsAssetRayPicker/HitAndMiss")
{
    VectorGraphicsAsset asset;
    asset.setWorldSize(glm::dvec2(2.0, 1.0));

    VectorGraphicsAssetRayPicker picker;
    CADacomb::AABB3              aabb = picker.getAABB(asset);
    CHECK(aabb.getMin().x == Catch::Approx(-1.0));
    CHECK(aabb.getMax().x == Catch::Approx(1.0));
    CHECK(aabb.getMin().y == Catch::Approx(-0.5));
    CHECK(aabb.getMax().y == Catch::Approx(0.5));

    CADacomb::Ray hitRay(glm::vec3(0.5f, 0.25f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    PickResult    hitRes = picker.pick(hitRay, asset);
    CHECK(hitRes.isHit());
    CHECK(hitRes.getDistance() == Catch::Approx(5.0));

    CADacomb::Ray missRay(glm::vec3(2.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    PickResult    missRes = picker.pick(missRay, asset);
    CHECK_FALSE(missRes.isHit());
}
#endif
