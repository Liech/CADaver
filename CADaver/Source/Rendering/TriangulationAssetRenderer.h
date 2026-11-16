#pragma once

#include "AssetMeshParams.h"
#include "AssetRenderer.h"
#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/MeshShaderParams.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace Vulcant
{
    class VulcantBuffer;
    class VulcantDevice;
    class VulcantGraphicCommand;
    class VulcantGraphicPipeline;
    class VulcantImage;
    class VulcantResource;
    class VulcantSet;
    class VulcantShader;
}

class AssetComponent;
class Renderer;
class TriangulationAsset;
class TriangulationSurfaceModule;

// Renders TriangulationAsset instances using modular Slang surface modifiers.
class TriangulationAssetRenderer : public AssetRenderer
{
  public:
    TriangulationAssetRenderer();
    TriangulationAssetRenderer(std::shared_ptr<const TriangulationAsset> asset);
    virtual ~TriangulationAssetRenderer();

    void setAsset(const std::shared_ptr<const Asset>& asset) override;
    void render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData) override;

    void rebuildVertices();

  private:
    void                        initializeResources(Renderer& renderer);
    void                        ensureModulePipeline(Renderer& renderer, TriangulationSurfaceModule& module);
    void                        invalidatePipelines();
    TriangulationSurfaceModule* resolveActiveModule(GatedDatabase& gatedDatabase, const AssetComponent*& outComponent) const;

    std::shared_ptr<const TriangulationAsset>                              asset;
    std::vector<std::unique_ptr<TriangulationSurfaceModule>>               surfaceModules;
    std::unique_ptr<Vulcant::VulcantShader>                                vertShader;
    std::map<std::string, std::unique_ptr<Vulcant::VulcantShader>>          fragShaders;
    std::map<std::string, std::unique_ptr<Vulcant::VulcantGraphicPipeline>> pipelines;
    std::map<std::string, std::unique_ptr<Vulcant::VulcantSet>>             descriptorSets;
    std::map<std::string, std::vector<Vulcant::VulcantResource*>>          lastSet1Resources;
    std::unique_ptr<Vulcant::VulcantBuffer>                                sceneDataUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>                                meshParamsUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>                                vertexBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer>                                indirectBuffer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>                        drawCmd;
    std::vector<Vulcant::Toolkit::MeshVertex>                              cachedVertices;
    AssetMeshParams                                                        meshParams{};
    glm::ivec2                                                             lastResolution = glm::ivec2(0, 0);
    Vulcant::VulcantCullMode                                               lastCullMode   = Vulcant::VulcantCullMode::None;
    glm::vec3                                                              lastColor      = glm::vec3(0.0f);
    size_t                                                                 lastRevision   = 0;
    uint32_t                                                               vertexCount    = 0;
    bool                                                                   lastSelected   = false;
    bool                                                                   dirty          = true;
};
