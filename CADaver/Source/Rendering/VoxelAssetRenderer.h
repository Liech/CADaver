#pragma once

#include "AssetMeshParams.h"
#include "AssetRenderer.h"
#include "CADacomb/Volume/LODStrategy/CapacityTargetLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/DistanceBandLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/FixedLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/FrustumAdaptiveLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/ScreenSpaceLODStrategy.h"
#include "CADacombRender/SVOLODMesh.h"
#include "CADacombRender/SVORaycast/Renderer/SVORaycastRenderer.h"
#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <memory>

namespace Vulcant
{
    class VulcantBuffer;
    class VulcantGraphicCommand;
    class VulcantGraphicPipeline;
    class VulcantSet;
    class VulcantShader;
}

class Renderer;
class VoxelAsset;

// Renders VoxelAsset instances using dynamic SVO LOD meshing or GPU SVO raycasting.
class VoxelAssetRenderer : public AssetRenderer
{
  public:
    VoxelAssetRenderer();
    explicit VoxelAssetRenderer(std::shared_ptr<const VoxelAsset> asset);
    virtual ~VoxelAssetRenderer();

    virtual void setAsset(const std::shared_ptr<const Asset>& asset) override;
    virtual void render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData) override;

    void rebuildMesh(Renderer& renderer, int chunkSize, int vertexCapacity);

  private:
    void initializeResources(Renderer& renderer);
    void ensurePipeline(Renderer& renderer);
    void invalidatePipeline();

    std::shared_ptr<const VoxelAsset>                     asset;
    std::unique_ptr<CADacomb::Render::SVOLODMesh<bool>>   lodMesh;
    std::unique_ptr<CADacomb::Render::SVORaycastRenderer> raycastRenderer;
    std::unique_ptr<Vulcant::VulcantShader>               vertShader;
    std::unique_ptr<Vulcant::VulcantShader>               fragShader;
    std::unique_ptr<Vulcant::VulcantGraphicPipeline>      pipeline;
    std::unique_ptr<Vulcant::VulcantSet>                  descriptorSet;
    std::unique_ptr<Vulcant::VulcantBuffer>               sceneDataUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>               meshParamsUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>               indirectBuffer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>       drawCmd;

    CADacomb::FrustumAdaptiveLODStrategy frustumAdaptiveStrategy;
    CADacomb::ScreenSpaceLODStrategy     screenSpaceStrategy;
    CADacomb::CapacityTargetLODStrategy  capacityStrategy;
    CADacomb::DistanceBandLODStrategy    distanceBandStrategy;
    CADacomb::FixedLODStrategy           fixedStrategy;

    AssetMeshParams          meshParams{};
    glm::ivec2               lastResolution     = glm::ivec2(0, 0);
    Vulcant::VulcantCullMode lastCullMode       = Vulcant::VulcantCullMode::None;
    int                      lastRenderMode     = -1;
    int                      lastChunkSize      = 32;
    int                      lastVertexCapacity = 2000000;
    size_t                   lastRevision       = 0;
    size_t                   lastRebuildCounter = 0;
    bool                     dirty              = true;
};
