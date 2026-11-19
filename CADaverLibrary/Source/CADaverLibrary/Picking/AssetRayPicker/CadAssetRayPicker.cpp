#include "Picking/AssetRayPicker/CadAssetRayPicker.h"

#include "CADacomb/CAD/Interface/Shape.h"
#include "Database/Asset/CadAsset.h"

CadAssetRayPicker::CadAssetRayPicker() {}

CadAssetRayPicker::~CadAssetRayPicker() {}

CADacomb::AABB3 CadAssetRayPicker::getAABB(const Asset& asset) const
{
    const auto& cadAsset = static_cast<const CadAsset&>(asset);
    if (!cadAsset.getTriangulation().vertices.empty())
    {
        return triangulationPicker.getAABB(cadAsset.getTriangulation());
    }

    return cadAsset.getCadShape().getAABB();
}

PickResult CadAssetRayPicker::pick(const CADacomb::Ray& ray, const Asset& asset) const
{
    const auto& cadAsset = static_cast<const CadAsset&>(asset);
    return triangulationPicker.pick(ray, cadAsset.getTriangulation(), cadAsset.getGuid(), cadAsset.getType());
}
