#include "Rendering/TriangulationAssetRenderer.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Database/AssetComponent.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Rendering/Renderer.h"
#include "Rendering/Triangulation/DefaultSurfaceModule.h"
#include "Rendering/Triangulation/IndexedFaceColorModule.h"
#include "Rendering/Triangulation/TriangulationSurfaceModule.h"
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

TriangulationAssetRenderer::TriangulationAssetRenderer()
{
    surfaceModules.push_back(std::make_unique<DefaultSurfaceModule>());
    surfaceModules.push_back(std::make_unique<IndexedFaceColorModule>());
}

TriangulationAssetRenderer::TriangulationAssetRenderer(std::shared_ptr<const TriangulationAsset> assetInput)
  : asset(std::move(assetInput))
{
    surfaceModules.push_back(std::make_unique<DefaultSurfaceModule>());
    surfaceModules.push_back(std::make_unique<IndexedFaceColorModule>());
}

TriangulationAssetRenderer::~TriangulationAssetRenderer()
{
    if (drawCmd)
    {
        drawCmd->wait();
        drawCmd.reset();
    }
    descriptorSets.clear();
    pipelines.clear();
    fragShaders.clear();
    vertShader.reset();
    surfaceModules.clear();
}

void TriangulationAssetRenderer::setAsset(const std::shared_ptr<const Asset>& newAsset)
{
    asset = std::dynamic_pointer_cast<const TriangulationAsset>(newAsset);
    dirty = true;
}

void TriangulationAssetRenderer::rebuildVertices()
{
    cachedVertices.clear();
    if (!asset || asset->getTriangulation().indices.empty())
    {
        vertexCount = 0;
        return;
    }

    const auto& triangulation = asset->getTriangulation();
    lastColor                 = asset->getMeshColor();
    glm::vec3 renderColor     = lastSelected ? glm::mix(lastColor, glm::vec3(1.0f, 0.65f, 0.1f), 0.65f) : lastColor;
    cachedVertices.reserve(triangulation.indices.size());

    for (size_t i = 0; i + 2 < triangulation.indices.size(); i += 3)
    {
        size_t idx0 = triangulation.indices[i];
        size_t idx1 = triangulation.indices[i + 1];
        size_t idx2 = triangulation.indices[i + 2];

        if (idx0 >= triangulation.vertices.size() || idx1 >= triangulation.vertices.size() || idx2 >= triangulation.vertices.size())
        {
            continue;
        }

        glm::vec3 p0 = glm::vec3(triangulation.vertices[idx0]);
        glm::vec3 p1 = glm::vec3(triangulation.vertices[idx1]);
        glm::vec3 p2 = glm::vec3(triangulation.vertices[idx2]);

        glm::vec3 normal = glm::cross(p1 - p0, p2 - p0);
        float     len    = glm::length(normal);
        if (len > 1e-6f)
        {
            normal /= len;
        }
        else
        {
            normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        cachedVertices.push_back({ p0, normal, renderColor });
        cachedVertices.push_back({ p1, normal, renderColor });
        cachedVertices.push_back({ p2, normal, renderColor });
    }

    vertexCount = static_cast<uint32_t>(cachedVertices.size());
}

void TriangulationAssetRenderer::initializeResources(Renderer& renderer)
{
    Vulcant::VulcantDevice& device = renderer.getDevice();
    sceneDataUbo                   = device.createUniform(1, sizeof(Vulcant::Toolkit::SceneData));
    meshParamsUbo                  = device.createUniform(1, sizeof(Vulcant::Toolkit::MeshShaderParams));
    indirectBuffer                 = device.createIndirectBuffer(1, sizeof(uint32_t) * 4);

    auto vertSpirv = std::vector<uint32_t>(Triangulation_vert_spirv, Triangulation_vert_spirv + Triangulation_vert_spirv_sizeInBytes / sizeof(uint32_t));
    vertShader     = device.createShader(vertSpirv);
    drawCmd        = device.createGraphicCommand();
}

void TriangulationAssetRenderer::invalidatePipelines()
{
    descriptorSets.clear();
    pipelines.clear();
    lastSet1Resources.clear();
}

void TriangulationAssetRenderer::ensureModulePipeline(Renderer& renderer, TriangulationSurfaceModule& module)
{
    Vulcant::VulcantDevice& device = renderer.getDevice();
    std::string             key    = module.getModuleKey();

    if (fragShaders.find(key) == fragShaders.end())
    {
        fragShaders[key] = device.createShader(module.getFragmentSpirv());
    }

    if (pipelines.find(key) == pipelines.end())
    {
        std::vector<Vulcant::VulcantShader*> shaders          = { fragShaders[key].get(), vertShader.get() };
        std::vector<Vulcant::VulcantImage*>  colorAttachments = { &renderer.getColor(), &renderer.getNormal() };

        Vulcant::VulcantRasterizationConfig rasterConfig;
        rasterConfig.setCullMode(lastCullMode);
        rasterConfig.setClear(false);

        pipelines[key] = device.createVulcanGraphicPipeline(shaders, colorAttachments, &renderer.getDepth(), nullptr, rasterConfig);
    }

    std::vector<std::shared_ptr<Vulcant::VulcantResource>> set1Shared = module.getSet1Resources();
    std::vector<Vulcant::VulcantResource*>                 set1Raw;
    set1Raw.reserve(set1Shared.size());
    for (const auto& res : set1Shared)
    {
        set1Raw.push_back(res.get());
    }

    if (descriptorSets.find(key) == descriptorSets.end() || lastSet1Resources[key] != set1Raw)
    {
        std::vector<std::vector<std::shared_ptr<Vulcant::VulcantResource>>> sets;
        sets.push_back({ sceneDataUbo->asResource(), meshParamsUbo->asResource() });
        if (!set1Shared.empty())
        {
            sets.push_back(set1Shared);
        }

        descriptorSets[key]    = device.createSet(sets, *fragShaders[key]);
        lastSet1Resources[key] = std::move(set1Raw);
    }
}

TriangulationSurfaceModule* TriangulationAssetRenderer::resolveActiveModule(GatedDatabase& gatedDatabase, const AssetComponent*& outComponent) const
{
    outComponent = nullptr;
    for (const auto& component : asset->getAttachments())
    {
        Polyglot::Node compSel = gatedDatabase.execute({
          {      "type",               "GetAssetSelection" },
          {   "version",                                 1 },
          { "assetGuid", component->getGuid().toString() }
        });
        if (compSel.toJson().value("type", "") == "GetAssetSelection")
        {
            for (const auto& module : surfaceModules)
            {
                if (module->isCompatible(component->getType()))
                {
                    outComponent = component.get();
                    return module.get();
                }
            }
        }
    }

    return surfaceModules.front().get();
}

void TriangulationAssetRenderer::render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData)
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
        invalidatePipelines();
        if (targetRes.x > 0 && targetRes.y > 0)
        {
            drawCmd = renderer.getDevice().createGraphicCommand();
        }
    }

    sceneDataUbo->uploadToGPU(&sceneData, 1);

    meshParams.wireframeColor = glm::vec4(renderPreferences.getWireframeColor(), 1.5f);
    meshParams.wireframeMode  = renderPreferences.getWireframeMode();
    meshParamsUbo->uploadToGPU(&meshParams, 1);

    Polyglot::Node selRes = gatedDatabase.execute({
      {      "type",   "GetAssetSelection" },
      {   "version",                     1 },
      { "assetGuid", asset->getGuid().toString() }
    });
    bool currentSelected = (selRes.toJson().value("type", "") == "GetAssetSelection");

    if (dirty || asset->getRevision() != lastRevision || lastColor != asset->getMeshColor() || currentSelected != lastSelected || !vertexBuffer)
    {
        lastColor    = asset->getMeshColor();
        lastSelected = currentSelected;
        rebuildVertices();
        if (vertexCount > 0)
        {
            vertexBuffer = renderer.getDevice().createVertexBuffer(vertexCount, sizeof(Vulcant::Toolkit::MeshVertex), false);
            vertexBuffer->uploadToGPU(cachedVertices.data(), vertexCount, 0);
            uint32_t indirectCmd[4] = { vertexCount, 1u, 0u, 0u };
            indirectBuffer->uploadToGPU(indirectCmd, 1);
        }
        else
        {
            vertexBuffer.reset();
        }
        dirty        = false;
        lastRevision = asset->getRevision();
    }

    if (vertexCount == 0 || !vertexBuffer || !drawCmd)
    {
        return;
    }

    const AssetComponent*       activeComponent = nullptr;
    TriangulationSurfaceModule* activeModule    = resolveActiveModule(gatedDatabase, activeComponent);
    activeModule->update(renderer.getDevice(), *asset, activeComponent);
    ensureModulePipeline(renderer, *activeModule);

    std::string key = activeModule->getModuleKey();
    drawCmd->wait();
    drawCmd->startRecord();
    drawCmd->beginRendering(*pipelines[key]);
    drawCmd->setViewportAndScissor(glm::uvec2(targetRes.x, targetRes.y));
    drawCmd->drawIndirect(*indirectBuffer, descriptorSets[key].get(), vertexBuffer.get());
    drawCmd->endRendering();
    drawCmd->endRecord();
    drawCmd->runAsync();
    drawCmd->wait();
}
