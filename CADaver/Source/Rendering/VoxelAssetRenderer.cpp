#include "VoxelAssetRenderer.h"

#include "Database/Asset/VoxelAsset.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/SVOPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Rendering/Renderer.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"

VoxelAssetRenderer::VoxelAssetRenderer() {}

VoxelAssetRenderer::VoxelAssetRenderer(std::shared_ptr<const VoxelAsset> assetInput)
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

void VoxelAssetRenderer::setAsset(const std::shared_ptr<const Asset>& newAsset)
{
    asset = std::dynamic_pointer_cast<const VoxelAsset>(newAsset);
    dirty = true;
}

void VoxelAssetRenderer::rebuildMesh(Renderer& renderer, int chunkSize, int vertexCapacity)
{
    if (!asset)
    {
        lodMesh.reset();
        dirty = false;
        return;
    }

    lastChunkSize      = chunkSize;
    lastVertexCapacity = vertexCapacity;

    lodMesh =
      std::make_unique<CADacomb::Render::SVOLODMesh<bool>>(renderer.getDevice(), asset->getSVO(), asset->getAABB(), static_cast<size_t>(lastVertexCapacity), static_cast<uint32_t>(lastChunkSize));

    dirty        = false;
    lastRevision = asset->getRevision();
}

void VoxelAssetRenderer::render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (!asset)
    {
        return;
    }

    Polyglot::Node svoPrefRes = gatedDatabase.execute({
      {    "type", "GetSVOPreferences" },
      { "version",                   1 }
    });
    SVOPreferences svoPrefs;
    if (svoPrefRes.contains("preferences"))
    {
        svoPrefs.fromNode(svoPrefRes["preferences"]);
    }

    if (dirty || asset->getRevision() != lastRevision || !lodMesh || lastChunkSize != svoPrefs.getChunkSize() || lastVertexCapacity != svoPrefs.getVertexCapacity() ||
        lastRebuildCounter != svoPrefs.getRebuildCounter())
    {
        rebuildMesh(renderer, svoPrefs.getChunkSize(), svoPrefs.getVertexCapacity());
        lastRebuildCounter = svoPrefs.getRebuildCounter();
    }

    if (!lodMesh)
    {
        return;
    }

    glm::ivec2 targetRes = renderer.getColor().getResolution();
    if (!meshRenderer)
    {
        meshRenderer = std::make_unique<Vulcant::Toolkit::MeshRenderer>(renderer.getDevice(), targetRes);
        meshRenderer->setClear(false);
        meshRenderer->setOutputTextures(renderer.getColor(), renderer.getDepth(), renderer.getNormal());
        drawCmd = renderer.getDevice().createGraphicCommand();
    }
    else if (meshRenderer->getResolution() != targetRes)
    {
        meshRenderer->setOutputTextures(renderer.getColor(), renderer.getDepth(), renderer.getNormal());
        drawCmd = renderer.getDevice().createGraphicCommand();
    }

    Polyglot::Node renderPrefRes = gatedDatabase.execute({
      {    "type", "GetRenderPreferences" },
      { "version",                      1 }
    });
    RenderPreferences renderPreferences;
    if (renderPrefRes.contains("preferences"))
    {
        renderPreferences.fromNode(renderPrefRes["preferences"]);
    }

    meshRenderer->setSceneData(sceneData);
    meshRenderer->setWireframeMode(static_cast<Vulcant::Toolkit::MeshRenderer::WireframeMode>(renderPreferences.getWireframeMode()));
    meshRenderer->setWireframeColor(renderPreferences.getWireframeColor());
    meshRenderer->setCullMode(static_cast<Vulcant::VulcantCullMode>(renderPreferences.getCullMode()));

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

    Polyglot::Node selRes = gatedDatabase.execute({
      {      "type",   "GetAssetSelection" },
      {   "version",                     1 },
      { "assetGuid", asset->getGuid().toString() }
    });
    bool      selected  = (selRes.toJson().value("type", "") == "GetAssetSelection");
    glm::vec3 meshColor = selected ? glm::vec3(1.0f, 0.65f, 0.1f) : glm::vec3(0.6f, 0.75f, 0.9f);
    lodMesh->setColor(meshColor);

    lodMesh->update(sceneData, *activeStrategy);

    if (lodMesh->getVertexCount() > 0 && drawCmd)
    {
        drawCmd->wait();
        drawCmd->startRecord();
        meshRenderer->record(*drawCmd, lodMesh->getVertexBuffer(), static_cast<uint32_t>(lodMesh->getVertexCount()));
        drawCmd->endRecord();
        drawCmd->runAsync();
        drawCmd->wait();
    }
}
