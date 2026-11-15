#pragma once

#include "AssetRenderer.h"
#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/MeshShaderParams.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace CADacomb::CAD
{
    class Edge;
    class Shape;
}

namespace Vulcant
{
    class VulcantBuffer;
    class VulcantGraphicCommand;
    class VulcantGraphicPipeline;
    class VulcantSet;
    class VulcantShader;
}

class CadAsset;
class Renderer;

// Renders CadAsset surfaces and CAD segment edge outlines using custom Slang pipelines.
class StepAssetRenderer : public AssetRenderer
{
  public:
    StepAssetRenderer();
    StepAssetRenderer(std::shared_ptr<const CadAsset> asset);
    virtual ~StepAssetRenderer();

    void setAsset(const std::shared_ptr<const Asset>& asset) override;
    void render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData) override;

    void rebuildGeometry();

  private:
    void initializeResources(Renderer& renderer);
    void ensurePipelines(Renderer& renderer);
    void invalidatePipelines();
    void collectShapeOutlines(const CADacomb::CAD::Shape& shape);
    void appendEdgeSegments(const CADacomb::CAD::Edge& edge);
    void appendOutlineQuad(const glm::vec3& p0, const glm::vec3& p1);

    std::shared_ptr<const CadAsset>                 asset;
    std::unique_ptr<Vulcant::VulcantShader>         surfaceVertShader;
    std::unique_ptr<Vulcant::VulcantShader>         surfaceFragShader;
    std::unique_ptr<Vulcant::VulcantGraphicPipeline> surfacePipeline;
    std::unique_ptr<Vulcant::VulcantSet>            surfaceDescriptorSet;
    std::unique_ptr<Vulcant::VulcantShader>         outlineVertShader;
    std::unique_ptr<Vulcant::VulcantShader>         outlineFragShader;
    std::unique_ptr<Vulcant::VulcantGraphicPipeline> outlinePipeline;
    std::unique_ptr<Vulcant::VulcantSet>            outlineDescriptorSet;
    std::unique_ptr<Vulcant::VulcantBuffer>         sceneDataUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>         meshParamsUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>         outlineParamsUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>         surfaceVertexBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer>         surfaceIndirectBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer>         outlineVertexBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer>         outlineIndirectBuffer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::vector<Vulcant::Toolkit::MeshVertex>       cachedSurfaceVertices;
    std::vector<Vulcant::Toolkit::MeshVertex>       cachedOutlineVertices;
    Vulcant::Toolkit::MeshShaderParams              meshParams{};
    glm::ivec2                                      lastResolution     = glm::ivec2(0, 0);
    Vulcant::VulcantCullMode                        lastCullMode       = Vulcant::VulcantCullMode::None;
    glm::vec3                                       lastColor          = glm::vec3(0.0f);
    size_t                                          lastRevision       = 0;
    uint32_t                                        surfaceVertexCount = 0;
    uint32_t                                        outlineVertexCount = 0;
    bool                                            lastSelected       = false;
    bool                                            dirty              = true;
};
