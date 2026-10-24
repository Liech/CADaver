#pragma once

#include "AssetRenderer.h"
#include "Vulcant/Toolkit/MeshRenderer.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class Renderer;
class TriangulationAsset;

// Renders TriangulationAsset instances into a Renderer with cached mesh vertices.
class TriangulationAssetRenderer : public AssetRenderer
{
  public:
    TriangulationAssetRenderer(std::shared_ptr<TriangulationAsset> asset);
    virtual ~TriangulationAssetRenderer();

    virtual void render(Renderer& renderer, bool isSelected = false) override;

    void rebuildVertices();

  private:
    std::shared_ptr<TriangulationAsset>       asset;
    std::vector<Vulcant::Toolkit::MeshVertex> cachedVertices;
    glm::vec3                                 lastColor = glm::vec3(0.0f);
    bool                                      dirty     = true;
};
