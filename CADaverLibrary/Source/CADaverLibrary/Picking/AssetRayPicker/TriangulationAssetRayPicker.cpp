#include "Picking/AssetRayPicker/TriangulationAssetRayPicker.h"

#include "Database/Asset/TriangulationAsset.h"

TriangulationAssetRayPicker::TriangulationAssetRayPicker() {}

TriangulationAssetRayPicker::~TriangulationAssetRayPicker() {}

CADacomb::AABB3 TriangulationAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto& triAsset = static_cast<const TriangulationAsset&>(asset);
    return triangulationPicker.getAABB(triAsset.getTriangulation());
}

PickResult TriangulationAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    const auto& triAsset = static_cast<const TriangulationAsset&>(asset);
    return triangulationPicker.pick(ray, triAsset.getTriangulation(), triAsset.getGuid(), triAsset.getType());
}
