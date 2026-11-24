#pragma once

#include "AssetMeshParams.h"
#include "AssetRenderer.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Vulcant
{
    class VulcantBuffer;
    class VulcantGraphicCommand;
    class VulcantGraphicPipeline;
    class VulcantImage;
    class VulcantSet;
    class VulcantShader;
}

class Renderer;
class TextureAsset;

// Renders TextureAsset instances as double-sided textured quads in 3D space.
class TextureAssetRenderer : public AssetRenderer
{
  public:
    TextureAssetRenderer();
    TextureAssetRenderer(std::shared_ptr<const TextureAsset> asset);
    virtual ~TextureAssetRenderer();

    void setAsset(const std::shared_ptr<const Asset>& asset) override;
    void render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData) override;

  private:
    void initializeResources(Renderer& renderer);
    void uploadQuadVertices();
    void uploadTextureImage(Renderer& renderer);
    void rebuildPipeline(Renderer& renderer);

    std::shared_ptr<const TextureAsset>              asset;
    std::unique_ptr<Vulcant::VulcantShader>          vertShader;
    std::unique_ptr<Vulcant::VulcantShader>          fragShader;
    std::unique_ptr<Vulcant::VulcantGraphicPipeline> pipeline;
    std::unique_ptr<Vulcant::VulcantSet>             descriptorSet;
    std::unique_ptr<Vulcant::VulcantBuffer>          sceneDataUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>          quadParamsUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>          vertexBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer>          indirectBuffer;
    std::unique_ptr<Vulcant::VulcantImage>           textureImage;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>  drawCmd;
    AssetMeshParams                                  quadParams{};
    glm::ivec2                                       lastResolution = glm::ivec2(0, 0);
    size_t                                           lastRevision   = 0;
    bool                                             dirty          = true;
};
