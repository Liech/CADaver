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

class Renderer;
class TriangulationAsset;

// Renders TriangulationAsset instances with a dedicated pipeline.
class TriangulationAssetRenderer : public AssetRenderer
{
  public:
    TriangulationAssetRenderer();
    TriangulationAssetRenderer(std::shared_ptr<TriangulationAsset> asset);
    virtual ~TriangulationAssetRenderer();

    virtual void setAsset(const std::shared_ptr<Asset>& asset) override;
    virtual void render(Renderer& renderer, bool isSelected = false) override;

    void rebuildVertices();

  private:
    std::shared_ptr<TriangulationAsset>             asset;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> meshRenderer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::vector<Vulcant::Toolkit::MeshVertex>       cachedVertices;
    glm::vec3                                       lastColor = glm::vec3(0.0f);
    bool                                            dirty     = true;
};
