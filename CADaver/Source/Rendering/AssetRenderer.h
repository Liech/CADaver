#pragma once

#include <memory>

class Asset;
class Renderer;

// Abstract base class for rendering project assets into a Renderer.
class AssetRenderer
{
  public:
    AssetRenderer();
    virtual ~AssetRenderer();

    virtual void setAsset(const std::shared_ptr<const Asset>& asset);
    virtual void render(Renderer& renderer) = 0;
};
