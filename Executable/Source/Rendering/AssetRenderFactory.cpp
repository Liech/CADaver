#include "AssetRenderFactory.h"

#include "Database/Asset.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/AssetType.h"
#include "StepAssetRenderer.h"
#include "TriangulationAssetRenderer.h"

AssetRenderFactory::AssetRenderFactory() {}

AssetRenderFactory::~AssetRenderFactory() {}

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
