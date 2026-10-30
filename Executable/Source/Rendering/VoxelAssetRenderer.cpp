#include "VoxelAssetRenderer.h"

#include "Commands/GatedDatabase.h"
#include "Database/Asset/VoxelAsset.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/SVOPreferences.h"
#include "Rendering/Renderer.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"

VoxelAssetRenderer::VoxelAssetRenderer() {}

VoxelAssetRenderer::VoxelAssetRenderer(std::shared_ptr<VoxelAsset> assetInput)
  : asset(std::move(assetInput))
{
}

VoxelAssetRenderer::~VoxelAssetRenderer()
{
    if (drawCmd)
    {
        drawCmd->wait();
        drawCmd.reset();
    }
    lodMesh.reset();
    meshRenderer.reset();
}

void VoxelAssetRenderer::setAsset(const std::shared_ptr<Asset>& newAsset)
{
    asset = std::dynamic_pointer_cast<VoxelAsset>(newAsset);
    dirty = true;
}

void VoxelAssetRenderer::rebuildMesh(Renderer& renderer)
{
    if (!asset || !asset->getSVO())
    {
        lodMesh.reset();
        dirty = false;
        return;
    }

    const auto& svoPrefs = GatedDatabase::getInstance().get().getPreferences().getSVOPreferences();
    lastChunkSize        = svoPrefs.getChunkSize();
    lastVertexCapacity   = svoPrefs.getVertexCapacity();

    lodMesh =
      std::make_unique<CADacomb::Render::SVOLODMesh<bool>>(renderer.getDevice(), *asset->getSVO(), asset->getAABB(), static_cast<size_t>(lastVertexCapacity), static_cast<uint32_t>(lastChunkSize));

    dirty = false;
    asset->markClean();
}

void VoxelAssetRenderer::render(Renderer& renderer, bool isSelected)
{
    if (!asset || !asset->getSVO())
    {
        return;
    }

    const auto& svoPrefs = GatedDatabase::getInstance().get().getPreferences().getSVOPreferences();
    if (dirty || asset->isDirty() || !lodMesh || lastChunkSize != svoPrefs.getChunkSize() || lastVertexCapacity != svoPrefs.getVertexCapacity() || svoPrefs.isRebuildRequested())
    {
        rebuildMesh(renderer);
        const_cast<SVOPreferences&>(svoPrefs).clearRebuildRequested();
    }

    if (!lodMesh)
    {
        return;
    }

    if (!meshRenderer)
    {
        meshRenderer = std::make_unique<Vulcant::Toolkit::MeshRenderer>(renderer.getDevice(), renderer.getResolution());
        meshRenderer->setClear(false);
        meshRenderer->setOutputTextures(renderer.getColor(), renderer.getDepth(), renderer.getNormal());
        drawCmd = renderer.getDevice().createGraphicCommand();
    }
    else if (meshRenderer->getResolution() != renderer.getResolution())
    {
        meshRenderer->setOutputTextures(renderer.getColor(), renderer.getDepth(), renderer.getNormal());
        drawCmd = renderer.getDevice().createGraphicCommand();
    }
    meshRenderer->setSceneData(renderer.getSceneData());
    meshRenderer->setWireframeMode(renderer.getWireframeMode());
    meshRenderer->setWireframeColor(renderer.getWireframeColor());
    meshRenderer->setCullMode(renderer.getCullMode());

    int                    strategyType   = svoPrefs.getStrategyType();
    CADacomb::LODStrategy* activeStrategy = &frustumAdaptiveStrategy;

    if (strategyType == 0)
    {
        frustumAdaptiveStrategy.setMinPixelSize(svoPrefs.getMinPixelSize());
        frustumAdaptiveStrategy.setMaxPixelSize(svoPrefs.getMaxPixelSize());
        frustumAdaptiveStrategy.setTargetFillRatio(svoPrefs.getTargetFillRatio());
        frustumAdaptiveStrategy.setMaxDepth(svoPrefs.getMaxDepth());
        frustumAdaptiveStrategy.setFrustumCullingEnabled(svoPrefs.isFrustumCullingEnabled());
        activeStrategy = &frustumAdaptiveStrategy;
    }
    else if (strategyType == 1)
    {
        screenSpaceStrategy.setMinPixelSize(svoPrefs.getMinPixelSize());
        screenSpaceStrategy.setMaxPixelSize(svoPrefs.getMaxPixelSize());
        activeStrategy = &screenSpaceStrategy;
    }
    else if (strategyType == 2)
    {
        capacityStrategy.setTargetFillRatio(svoPrefs.getTargetFillRatio());
        activeStrategy = &capacityStrategy;
    }
    else if (strategyType == 3)
    {
        distanceBandStrategy.setNearDistance(svoPrefs.getNearDistance());
        distanceBandStrategy.setFarDistance(svoPrefs.getFarDistance());
        activeStrategy = &distanceBandStrategy;
    }
    else
    {
        int dec = svoPrefs.getFixedDecision();
        fixedStrategy.setDecision(dec == 0 ? CADacomb::LODDecision::Refine : (dec == 2 ? CADacomb::LODDecision::Coarsen : CADacomb::LODDecision::Keep));
        activeStrategy = &fixedStrategy;
    }

    glm::vec3 meshColor(0.6f, 0.75f, 0.9f);
    lodMesh->setColor(meshColor);

    lodMesh->update(renderer.getSceneData(), *activeStrategy);

    if (lodMesh->getVertexCount() > 0 && drawCmd)
    {
        drawCmd->wait();
        drawCmd->startRecord();
        drawCmd->setClearColor(renderer.getClearColor());
        meshRenderer->record(*drawCmd, lodMesh->getVertexBuffer(), static_cast<uint32_t>(lodMesh->getVertexCount()));
        drawCmd->endRecord();
        drawCmd->runAsync();
        drawCmd->wait();
    }
}
