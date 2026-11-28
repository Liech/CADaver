#pragma once

#include "AssetRenderer.h"
#include "CADacombRender/SDFRaycast/SdfRaycastRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <memory>
#include <string>

class Renderer;
class SdfAsset;

// Renders SdfAsset instances using GPU sphere-tracing via CADacomb::Render::SdfRaycastRenderer.
class SdfAssetRenderer : public AssetRenderer
{
  public:
    SdfAssetRenderer();
    explicit SdfAssetRenderer(std::shared_ptr<const SdfAsset> asset);
    virtual ~SdfAssetRenderer();

    virtual void setAsset(const std::shared_ptr<const Asset>& asset) override;
    virtual void render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData) override;

  private:
    std::shared_ptr<const SdfAsset>                       asset;
    std::unique_ptr<CADacomb::Render::SdfRaycastRenderer> raycastRenderer;
    std::string                                           lastFormulaName;
    std::string                                           lastFormulaHelper;
    std::string                                           lastFormulaExpression;
    bool                                                  dirty = true;
};
