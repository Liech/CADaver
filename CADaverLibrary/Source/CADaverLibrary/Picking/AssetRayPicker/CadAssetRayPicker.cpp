#include "Picking/AssetRayPicker/CadAssetRayPicker.h"

#include "Database/Asset/CadAsset.h"

CadAssetRayPicker::CadAssetRayPicker() {}

CadAssetRayPicker::~CadAssetRayPicker() {}

CADacomb::AABB3 CadAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto& cadAsset = static_cast<const CadAsset&>(asset);
    return triangulationPicker.getAABB(cadAsset.getTriangulation());
}

PickResult CadAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    const auto& cadAsset = static_cast<const CadAsset&>(asset);
    return triangulationPicker.pick(ray, cadAsset.getTriangulation(), cadAsset.getGuid(), cadAsset.getType());
}
