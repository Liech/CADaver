#pragma once

#include "AssetRenderer.h"

#include <memory>
#include <unordered_map>

class Asset;
class Renderer;

// Factory and manager for creating and executing asset renderers.
class AssetRenderFactory
{
  public:
    AssetRenderFactory();
    virtual ~AssetRenderFactory();

    static AssetRenderFactory& getInstance();

    std::unique_ptr<AssetRenderer> create(const std::shared_ptr<Asset>& asset);
    void                           render(Renderer& renderer, const std::shared_ptr<Asset>& asset, bool isSelected = false);
    void                           clearCache();

  private:
    std::unordered_map<const Asset*, std::shared_ptr<AssetRenderer>> rendererCache;
};
