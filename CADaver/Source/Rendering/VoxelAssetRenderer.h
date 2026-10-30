#pragma once

#include "AssetRenderer.h"
#include "CADacomb/Volume/LODStrategy/CapacityTargetLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/DistanceBandLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/FixedLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/FrustumAdaptiveLODStrategy.h"
#include "CADacomb/Volume/LODStrategy/ScreenSpaceLODStrategy.h"
#include "CADacombRender/SVOLODMesh.h"
#include "Vulcant/Toolkit/MeshRenderer.h"

#include <memory>

namespace Vulcant
{
    class VulcantGraphicCommand;
}

class Renderer;
class VoxelAsset;

// Renders VoxelAsset instances using dynamic SVO LOD meshing with a dedicated pipeline.
class VoxelAssetRenderer : public AssetRenderer
{
  public:
    VoxelAssetRenderer();
    explicit VoxelAssetRenderer(std::shared_ptr<const VoxelAsset> asset);
    virtual ~VoxelAssetRenderer();

    virtual void setAsset(const std::shared_ptr<const Asset>& asset) override;
    virtual void render(Renderer& renderer, bool isSelected = false) override;

    void rebuildMesh(Renderer& renderer);

  private:
    std::shared_ptr<const VoxelAsset>                   asset;
    std::unique_ptr<CADacomb::Render::SVOLODMesh<bool>> lodMesh;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer>     meshRenderer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>     drawCmd;

    CADacomb::FrustumAdaptiveLODStrategy frustumAdaptiveStrategy;
    CADacomb::ScreenSpaceLODStrategy     screenSpaceStrategy;
    CADacomb::CapacityTargetLODStrategy  capacityStrategy;
    CADacomb::DistanceBandLODStrategy    distanceBandStrategy;
    CADacomb::FixedLODStrategy           fixedStrategy;

    int    lastChunkSize      = 32;
    int    lastVertexCapacity = 500000;
    size_t lastRevision       = 0;
    bool   dirty              = true;
};
