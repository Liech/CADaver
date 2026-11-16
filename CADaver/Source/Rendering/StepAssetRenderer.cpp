#include "StepAssetRenderer.h"

#include "CAD/CadOutline_frag.h"
#include "CAD/CadOutline_vert.h"
#include "CADacomb/CAD/Interface/CurveType.h"
#include "CADacomb/CAD/Interface/Edge.h"
#include "CADacomb/CAD/Interface/Face.h"
#include "CADacomb/CAD/Interface/Shape.h"
#include "CADacomb/CAD/Interface/Wire.h"
#include "Database/Asset/CadAsset.h"
#include "Database/Preferences/RenderPreferences.h"
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

#include <array>

StepAssetRenderer::StepAssetRenderer() {}

StepAssetRenderer::StepAssetRenderer(std::shared_ptr<const CadAsset> assetInput)
  : asset(std::move(assetInput))
{
}

StepAssetRenderer::~StepAssetRenderer()
{
    if (drawCmd)
    {
        drawCmd->wait();
        drawCmd.reset();
    }
    surfaceDescriptorSet.reset();
    surfacePipeline.reset();
    outlineDescriptorSet.reset();
    outlinePipeline.reset();
}

void StepAssetRenderer::setAsset(const std::shared_ptr<const Asset>& newAsset)
{
    asset = std::dynamic_pointer_cast<const CadAsset>(newAsset);
    dirty = true;
}

void StepAssetRenderer::initializeResources(Renderer& renderer)
{
    Vulcant::VulcantDevice& device = renderer.getDevice();
    sceneDataUbo                   = device.createUniform(1, sizeof(Vulcant::Toolkit::SceneData));
    meshParamsUbo                  = device.createUniform(1, sizeof(AssetMeshParams));
    outlineParamsUbo               = device.createUniform(1, sizeof(AssetOutlineParams));
    surfaceIndirectBuffer          = device.createIndirectBuffer(1, sizeof(uint32_t) * 4);
    outlineIndirectBuffer          = device.createIndirectBuffer(1, sizeof(uint32_t) * 4);

    auto sVertSpirv   = std::vector<uint32_t>(Triangulation_vert_spirv, Triangulation_vert_spirv + Triangulation_vert_spirv_sizeInBytes / sizeof(uint32_t));
    auto sFragSpirv   = std::vector<uint32_t>(Triangulation_Default_frag_spirv, Triangulation_Default_frag_spirv + Triangulation_Default_frag_spirv_sizeInBytes / sizeof(uint32_t));
    surfaceVertShader = device.createShader(sVertSpirv);
    surfaceFragShader = device.createShader(sFragSpirv);

    auto oVertSpirv   = std::vector<uint32_t>(CadOutline_vert_spirv, CadOutline_vert_spirv + CadOutline_vert_spirv_sizeInBytes / sizeof(uint32_t));
    auto oFragSpirv   = std::vector<uint32_t>(CadOutline_frag_spirv, CadOutline_frag_spirv + CadOutline_frag_spirv_sizeInBytes / sizeof(uint32_t));
    outlineVertShader = device.createShader(oVertSpirv);
    outlineFragShader = device.createShader(oFragSpirv);

    drawCmd = device.createGraphicCommand();
}

void StepAssetRenderer::invalidatePipelines()
{
    surfaceDescriptorSet.reset();
    surfacePipeline.reset();
    outlineDescriptorSet.reset();
    outlinePipeline.reset();
}

void StepAssetRenderer::ensurePipelines(Renderer& renderer)
{
    Vulcant::VulcantDevice&             device           = renderer.getDevice();
    std::vector<Vulcant::VulcantImage*> colorAttachments = { &renderer.getColor(), &renderer.getNormal() };

    if (!surfacePipeline)
    {
        std::vector<Vulcant::VulcantShader*> shaders = { surfaceFragShader.get(), surfaceVertShader.get() };
        Vulcant::VulcantRasterizationConfig   rasterConfig;
        rasterConfig.setCullMode(lastCullMode);
        rasterConfig.setClear(false);
        surfacePipeline = device.createVulcanGraphicPipeline(shaders, colorAttachments, &renderer.getDepth(), nullptr, rasterConfig);
    }

    if (!surfaceDescriptorSet)
    {
        std::vector<std::vector<std::shared_ptr<Vulcant::VulcantResource>>> sets;
        sets.push_back({ sceneDataUbo->asResource(), meshParamsUbo->asResource() });
        surfaceDescriptorSet = device.createSet(sets, *surfaceFragShader);
    }

    if (!outlinePipeline)
    {
        std::vector<Vulcant::VulcantShader*> shaders = { outlineFragShader.get(), outlineVertShader.get() };
        Vulcant::VulcantRasterizationConfig   rasterConfig;
        rasterConfig.setCullMode(Vulcant::VulcantCullMode::None);
        rasterConfig.setClear(false);
        outlinePipeline = device.createVulcanGraphicPipeline(shaders, colorAttachments, &renderer.getDepth(), nullptr, rasterConfig);
    }

    if (!outlineDescriptorSet)
    {
        std::vector<std::vector<std::shared_ptr<Vulcant::VulcantResource>>> sets;
        sets.push_back({ sceneDataUbo->asResource(), outlineParamsUbo->asResource() });
        outlineDescriptorSet = device.createSet(sets, *outlineFragShader);
    }
}

void StepAssetRenderer::appendOutlineQuad(const glm::vec3& p0, const glm::vec3& p1)
{
    if (glm::length(p1 - p0) < 1e-7f)
    {
        return;
    }

    Vulcant::Toolkit::MeshVertex v00{ p0, p1, glm::vec3(-1.0f, 0.0f, 0.0f) };
    Vulcant::Toolkit::MeshVertex v01{ p0, p1, glm::vec3(1.0f, 0.0f, 0.0f) };
    Vulcant::Toolkit::MeshVertex v10{ p1, p0, glm::vec3(1.0f, 0.0f, 0.0f) };
    Vulcant::Toolkit::MeshVertex v11{ p1, p0, glm::vec3(-1.0f, 0.0f, 0.0f) };

    cachedOutlineVertices.push_back(v00);
    cachedOutlineVertices.push_back(v01);
    cachedOutlineVertices.push_back(v11);

    cachedOutlineVertices.push_back(v00);
    cachedOutlineVertices.push_back(v11);
    cachedOutlineVertices.push_back(v10);
}

void StepAssetRenderer::appendEdgeSegments(const CADacomb::CAD::Edge& edge)
{
    std::pair<double, double> range    = edge.getParameterRange();
    size_t                    segments = (edge.getCurveType() == CADacomb::CAD::CurveType::Line) ? 1 : 24;
    double                    tStart   = range.first;
    double                    tSpan    = range.second - range.first;

    glm::vec3 prev = glm::vec3(edge.evaluatePoint(tStart));
    for (size_t i = 1; i <= segments; i++)
    {
        double    t    = tStart + tSpan * (static_cast<double>(i) / static_cast<double>(segments));
        glm::vec3 curr = glm::vec3(edge.evaluatePoint(t));
        appendOutlineQuad(prev, curr);
        prev = curr;
    }
}

void StepAssetRenderer::collectShapeOutlines(const CADacomb::CAD::Shape& shape)
{
    if (const auto* edge = dynamic_cast<const CADacomb::CAD::Edge*>(&shape))
    {
        appendEdgeSegments(*edge);
        return;
    }

    if (const auto* face = dynamic_cast<const CADacomb::CAD::Face*>(&shape))
    {
        for (const auto& wire : face->getWires())
        {
            for (const auto& wireEdge : wire->getEdges())
            {
                appendEdgeSegments(*wireEdge);
            }
        }
        return;
    }

    for (const auto& child : shape.getChildren())
    {
        collectShapeOutlines(*child);
    }
}

void StepAssetRenderer::rebuildGeometry()
{
    cachedSurfaceVertices.clear();
    cachedOutlineVertices.clear();
    surfaceVertexCount = 0;
    outlineVertexCount = 0;

    if (!asset)
    {
        return;
    }

    const auto& triangulation = asset->getTriangulation();
    lastColor                 = asset->getMeshColor();
    glm::vec3 renderColor     = lastSelected ? glm::mix(lastColor, glm::vec3(1.0f, 0.65f, 0.1f), 0.65f) : lastColor;

    if (!triangulation.indices.empty())
    {
        cachedSurfaceVertices.reserve(triangulation.indices.size());

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

            cachedSurfaceVertices.push_back({ p0, normal, renderColor });
            cachedSurfaceVertices.push_back({ p1, normal, renderColor });
            cachedSurfaceVertices.push_back({ p2, normal, renderColor });
        }
    }

    collectShapeOutlines(asset->getCadShape());

    surfaceVertexCount = static_cast<uint32_t>(cachedSurfaceVertices.size());
    outlineVertexCount = static_cast<uint32_t>(cachedOutlineVertices.size());
}

void StepAssetRenderer::render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (!asset)
    {
        return;
    }

    if (!surfaceVertShader)
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

    meshParams.params.wireframeColor = glm::vec4(renderPreferences.getWireframeColor(), 1.5f);
    meshParams.params.wireframeMode  = renderPreferences.getWireframeMode();
    meshParams.setTransformation(asset->getTransformation());
    meshParamsUbo->uploadToGPU(&meshParams, 1);

    AssetOutlineParams outlineParams;
    outlineParams.viewportSize = glm::vec2(targetRes);
    outlineParams.setTransformation(asset->getTransformation());
    outlineParamsUbo->uploadToGPU(&outlineParams, 1);

    Polyglot::Node selRes = gatedDatabase.execute({
      {      "type",   "GetAssetSelection" },
      {   "version",                     1 },
      { "assetGuid", asset->getGuid().toString() }
    });
    bool currentSelected = (selRes.toJson().value("type", "") == "GetAssetSelection");

    if (dirty || asset->getRevision() != lastRevision || lastColor != asset->getMeshColor() || currentSelected != lastSelected)
    {
        lastColor    = asset->getMeshColor();
        lastSelected = currentSelected;
        rebuildGeometry();

        if (surfaceVertexCount > 0)
        {
            surfaceVertexBuffer = renderer.getDevice().createVertexBuffer(surfaceVertexCount, sizeof(Vulcant::Toolkit::MeshVertex), false);
            surfaceVertexBuffer->uploadToGPU(cachedSurfaceVertices.data(), surfaceVertexCount, 0);
            uint32_t indirectCmd[4] = { surfaceVertexCount, 1u, 0u, 0u };
            surfaceIndirectBuffer->uploadToGPU(indirectCmd, 1);
        }
        else
        {
            surfaceVertexBuffer.reset();
        }

        if (outlineVertexCount > 0)
        {
            outlineVertexBuffer = renderer.getDevice().createVertexBuffer(outlineVertexCount, sizeof(Vulcant::Toolkit::MeshVertex), false);
            outlineVertexBuffer->uploadToGPU(cachedOutlineVertices.data(), outlineVertexCount, 0);
            uint32_t indirectCmd[4] = { outlineVertexCount, 1u, 0u, 0u };
            outlineIndirectBuffer->uploadToGPU(indirectCmd, 1);
        }
        else
        {
            outlineVertexBuffer.reset();
        }

        dirty        = false;
        lastRevision = asset->getRevision();
    }

    if ((surfaceVertexCount == 0 && outlineVertexCount == 0) || !drawCmd)
    {
        return;
    }

    ensurePipelines(renderer);

    if (surfaceVertexCount > 0 && surfaceVertexBuffer)
    {
        drawCmd->wait();
        drawCmd->startRecord();
        drawCmd->beginRendering(*surfacePipeline);
        drawCmd->setViewportAndScissor(glm::uvec2(targetRes.x, targetRes.y));
        drawCmd->drawIndirect(*surfaceIndirectBuffer, surfaceDescriptorSet.get(), surfaceVertexBuffer.get());
        drawCmd->endRendering();
        drawCmd->endRecord();
        drawCmd->runAsync();
        drawCmd->wait();
    }
    if (outlineVertexCount > 0 && outlineVertexBuffer)
    {
        drawCmd->wait();
        drawCmd->startRecord();
        drawCmd->beginRendering(*outlinePipeline);
        drawCmd->setViewportAndScissor(glm::uvec2(targetRes.x, targetRes.y));
        drawCmd->drawIndirect(*outlineIndirectBuffer, outlineDescriptorSet.get(), outlineVertexBuffer.get());
        drawCmd->endRendering();
        drawCmd->endRecord();
        drawCmd->runAsync();
        drawCmd->wait();
    }
}
