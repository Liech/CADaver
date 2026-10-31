#pragma once

#include "AssetRenderer.h"
#include "Vulcant/Toolkit/MeshRenderer.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Vulcant
{
    class VulcantGraphicCommand;
}

class CadAsset;
class Renderer;

// Renders CadAsset instances with a dedicated pipeline.
class StepAssetRenderer : public AssetRenderer
{
  public:
    StepAssetRenderer();
    StepAssetRenderer(std::shared_ptr<const CadAsset> asset);
    virtual ~StepAssetRenderer();

    virtual void setAsset(const std::shared_ptr<const Asset>& asset) override;
    virtual void render(Renderer& renderer) override;

    void rebuildVertices();

  private:
    std::shared_ptr<const CadAsset>                 asset;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> meshRenderer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::vector<Vulcant::Toolkit::MeshVertex>       cachedVertices;
    glm::vec3                                       lastColor    = glm::vec3(0.0f);
    size_t                                          lastRevision = 0;
    bool                                            dirty        = true;
};
