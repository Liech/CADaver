#include "AssetRenderFactory.h"

#include "Database/Asset.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/AssetType.h"
#include "StepAssetRenderer.h"
#include "TriangulationAssetRenderer.h"
#include "VoxelAssetRenderer.h"

AssetRenderFactory::AssetRenderFactory() {}

AssetRenderFactory::~AssetRenderFactory() {}

AssetRenderFactory& AssetRenderFactory::getInstance()
{
    static AssetRenderFactory instance;
    return instance;
}

std::unique_ptr<AssetRenderer> AssetRenderFactory::create(const std::shared_ptr<Asset>& asset)
{
    if (!asset)
    {
        return nullptr;
    }

    if (asset->getType() == AssetType::Triangulation)
    {
        auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
        if (triAsset)
        {
            return std::make_unique<TriangulationAssetRenderer>(triAsset);
        }
    }
    else if (asset->getType() == AssetType::Cad)
    {
        auto cadAsset = std::dynamic_pointer_cast<CadAsset>(asset);
        if (cadAsset)
        {
            return std::make_unique<StepAssetRenderer>(cadAsset);
        }
    }
    else if (asset->getType() == AssetType::Voxel)
    {
        auto voxelAsset = std::dynamic_pointer_cast<VoxelAsset>(asset);
        if (voxelAsset)
        {
            return std::make_unique<VoxelAssetRenderer>(voxelAsset);
        }
    }

    return nullptr;
}

void AssetRenderFactory::render(Renderer& renderer, const std::shared_ptr<Asset>& asset, bool isSelected)
{
    if (!asset)
    {
        return;
    }

    auto it = rendererCache.find(asset.get());
    if (it == rendererCache.end())
    {
        auto newRenderer = create(asset);
        if (newRenderer)
        {
            it = rendererCache.emplace(asset.get(), std::move(newRenderer)).first;
        }
    }

    if (it != rendererCache.end() && it->second)
    {
        it->second->render(renderer, isSelected);
    }
}

void AssetRenderFactory::clearCache()
{
    rendererCache.clear();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetRenderFactory/SingletonInstance")
{
    auto& instance1 = AssetRenderFactory::getInstance();
    auto& instance2 = AssetRenderFactory::getInstance();
    CHECK(&instance1 == &instance2);
}
#endif
