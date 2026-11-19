#include "VoxelAssetRenderer.h"

#include "Database/Asset/VoxelAsset.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/SVOPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Rendering/Renderer.h"
#include "Triangulation/Triangulation_Default_frag.h"
#include "Triangulation/Triangulation_vert.h"
#include "Vulcant/Interface/VulcantBuffer.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantGraphicPipeline.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantRasterizationConfig.h"
#include "Vulcant/Interface/VulcantResource.h"
#include "Vulcant/Interface/VulcantSet.h"
#include "Vulcant/Interface/VulcantShader.h"

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
    descriptorSet.reset();
    pipeline.reset();
    fragShader.reset();
    vertShader.reset();
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

void VoxelAssetRenderer::initializeResources(Renderer& renderer)
{
    Vulcant::VulcantDevice& device = renderer.getDevice();
    sceneDataUbo                   = device.createUniform(1, sizeof(Vulcant::Toolkit::SceneData));
    meshParamsUbo                  = device.createUniform(1, sizeof(AssetMeshParams));
    indirectBuffer                 = device.createIndirectBuffer(1, sizeof(uint32_t) * 4);

    auto vertSpirv = std::vector<uint32_t>(Triangulation_vert_spirv, Triangulation_vert_spirv + Triangulation_vert_spirv_sizeInBytes / sizeof(uint32_t));
    auto fragSpirv = std::vector<uint32_t>(Triangulation_Default_frag_spirv, Triangulation_Default_frag_spirv + Triangulation_Default_frag_spirv_sizeInBytes / sizeof(uint32_t));
    vertShader     = device.createShader(vertSpirv);
    fragShader     = device.createShader(fragSpirv);
    drawCmd        = device.createGraphicCommand();
}

void VoxelAssetRenderer::invalidatePipeline()
{
    descriptorSet.reset();
    pipeline.reset();
}

void VoxelAssetRenderer::ensurePipeline(Renderer& renderer)
{
    Vulcant::VulcantDevice& device = renderer.getDevice();

    if (!pipeline)
    {
        std::vector<Vulcant::VulcantShader*> shaders          = { fragShader.get(), vertShader.get() };
        std::vector<Vulcant::VulcantImage*>  colorAttachments = { &renderer.getColor(), &renderer.getNormal() };

        Vulcant::VulcantRasterizationConfig rasterConfig;
        rasterConfig.setCullMode(lastCullMode);
        rasterConfig.setClear(false);

        pipeline = device.createVulcanGraphicPipeline(shaders, colorAttachments, &renderer.getDepth(), nullptr, rasterConfig);
    }

    if (!descriptorSet)
    {
        std::vector<std::vector<std::shared_ptr<Vulcant::VulcantResource>>> sets;
        sets.push_back({ sceneDataUbo->asResource(), meshParamsUbo->asResource() });
        descriptorSet = device.createSet(sets, *fragShader);
    }
}

void VoxelAssetRenderer::render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& worldSceneData)
{
    if (!asset)
    {
        return;
    }

    if (!vertShader)
    {
        initializeResources(renderer);
        dirty = true;
    }

    glm::mat4                   model          = glm::mat4(asset->getTransformation());
    Vulcant::Toolkit::SceneData localSceneData = worldSceneData;
    localSceneData.viewMatrix                  = worldSceneData.viewMatrix * model;
    localSceneData.invViewMatrix               = glm::inverse(localSceneData.viewMatrix);
    localSceneData.cameraPos                   = glm::vec4(glm::vec3(localSceneData.invViewMatrix[3]), 1.0f);

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

    Polyglot::Node renderPrefRes = gatedDatabase.execute({
      {    "type", "GetRenderPreferences" },
      { "version",                      1 }
    });
    RenderPreferences renderPreferences;
    if (renderPrefRes.contains("preferences"))
    {
        renderPreferences.fromNode(renderPrefRes["preferences"]);
    }

    glm::ivec2               targetRes      = renderer.getColor().getResolution();
    Vulcant::VulcantCullMode targetCullMode = static_cast<Vulcant::VulcantCullMode>(renderPreferences.getCullMode());
    if (targetRes != lastResolution || targetCullMode != lastCullMode)
    {
        lastResolution = targetRes;
        lastCullMode   = targetCullMode;
        invalidatePipeline();
        if (targetRes.x > 0 && targetRes.y > 0)
        {
            drawCmd = renderer.getDevice().createGraphicCommand();
        }
    }

    sceneDataUbo->uploadToGPU(&worldSceneData, 1);

    meshParams.params.wireframeColor = glm::vec4(renderPreferences.getWireframeColor(), 1.5f);
    meshParams.params.wireframeMode  = renderPreferences.getWireframeMode();
    meshParams.setTransformation(asset->getTransformation());
    meshParamsUbo->uploadToGPU(&meshParams, 1);

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

    lodMesh->update(localSceneData, *activeStrategy);

    uint32_t vertexCount = static_cast<uint32_t>(lodMesh->getVertexCount());
    if (vertexCount > 0 && drawCmd)
    {
        ensurePipeline(renderer);
        uint32_t indirectCmd[4] = { vertexCount, 1u, 0u, 0u };
        indirectBuffer->uploadToGPU(indirectCmd, 1);

        drawCmd->wait();
        drawCmd->startRecord();
        drawCmd->beginRendering(*pipeline);
        drawCmd->setViewportAndScissor(glm::uvec2(targetRes.x, targetRes.y));
        drawCmd->drawIndirect(*indirectBuffer, descriptorSet.get(), &lodMesh->getVertexBuffer());
        drawCmd->endRendering();
        drawCmd->endRecord();
        drawCmd->runAsync();
        drawCmd->wait();
    }
}
