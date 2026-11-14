#pragma once

#include "Vulcant/Toolkit/SceneData.h"

#include <memory>

class Asset;
class GatedDatabase;
class Renderer;

// Abstract base class for rendering project assets into a Renderer.
class AssetRenderer
{
  public:
    AssetRenderer();
    virtual ~AssetRenderer();

    virtual void setAsset(const std::shared_ptr<const Asset>& asset);
    virtual void render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData) = 0;
};
