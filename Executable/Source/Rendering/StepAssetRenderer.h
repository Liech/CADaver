#pragma once

#include "AssetRenderer.h"
#include "Vulcant/Toolkit/MeshRenderer.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class CadAsset;
class Renderer;

// Renders CadAsset instances into a Renderer with cached triangulated mesh vertices.
class StepAssetRenderer : public AssetRenderer
{
  public:
    StepAssetRenderer(std::shared_ptr<CadAsset> asset);
    virtual ~StepAssetRenderer();

    virtual void render(Renderer& renderer, bool isSelected = false) override;

    void rebuildVertices();

  private:
    std::shared_ptr<CadAsset>                 asset;
    std::vector<Vulcant::Toolkit::MeshVertex> cachedVertices;
    glm::vec3                                 lastColor = glm::vec3(0.0f);
    bool                                      dirty     = true;
};
