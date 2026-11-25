#include "Rendering/VectorGraphicsAssetRenderer.h"

#include "CADacomb/Texture/Texture.h"
#include "CADacomb/VectorGraphics/Rasterizer/VectorGraphicsRasterizer.h"
#include "CADacombRender/Texture/TextureImageConverter.h"
#include "Database/Asset/VectorGraphicsAsset.h"
#include "Execution/GatedDatabase.h"
#include "Rendering/Renderer.h"
#include "Texture/TextureQuad_frag.h"
#include "Texture/TextureQuad_vert.h"
#include "Vulcant/Interface/VulcantBuffer.h"
#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantGraphicPipeline.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantImageUsage.h"
#include "Vulcant/Interface/VulcantRasterizationConfig.h"
#include "Vulcant/Interface/VulcantResource.h"
#include "Vulcant/Interface/VulcantSet.h"
#include "Vulcant/Interface/VulcantShader.h"

#include <algorithm>
#include <cmath>

VectorGraphicsAssetRenderer::VectorGraphicsAssetRenderer() {}

VectorGraphicsAssetRenderer::VectorGraphicsAssetRenderer(std::shared_ptr<const VectorGraphicsAsset> assetInput)
  : asset(std::move(assetInput))
{
}

VectorGraphicsAssetRenderer::~VectorGraphicsAssetRenderer()
{
    if (drawCmd)
    {
        drawCmd->wait();
        drawCmd.reset();
    }
    descriptorSet.reset();
    pipeline.reset();
    fragShader.reset();
    vertShader.reset();
    textureImage.reset();
}

void VectorGraphicsAssetRenderer::setAsset(const std::shared_ptr<const Asset>& newAsset)
{
    asset = std::dynamic_pointer_cast<const VectorGraphicsAsset>(newAsset);
    dirty = true;
}

void VectorGraphicsAssetRenderer::render(Renderer& renderer, GatedDatabase& gatedDatabase, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (!asset || (asset->getVectorGraphics().isEmpty() && !asset->getVectorGraphics().hasViewBox()))
    {
        return;
    }

    if (!vertShader)
    {
        initializeResources(renderer);
    }

    if (dirty || asset->getRevision() != lastRevision)
    {
        uploadQuadVertices();
        uploadRasterizedImage(renderer);
        lastRevision = asset->getRevision();
        dirty        = false;
    }

    glm::ivec2 currentResolution = renderer.getColor().getResolution();
    if (!pipeline || currentResolution != lastResolution)
    {
        rebuildPipeline(renderer);
        lastResolution = currentResolution;
    }

    if (!textureImage || !descriptorSet || !pipeline || !drawCmd)
    {
        return;
    }

    sceneDataUbo->uploadToGPU(&sceneData, 1);

    Polyglot::Node selRes     = gatedDatabase.execute({
      {      "type",         "GetAssetSelection" },
      {   "version",                           1 },
      { "assetGuid", asset->getGuid().toString() }
    });
    bool           isSelected = (selRes.toJson().value("type", "") == "GetAssetSelection");

    quadParams.params.wireframeColor = glm::vec4(1.0f, 0.65f, 0.1f, 1.0f);
    quadParams.params.wireframeMode  = isSelected ? 1 : 0;
    quadParams.setTransformation(asset->getTransformation());
    quadParamsUbo->uploadToGPU(&quadParams, 1);

    drawCmd->wait();
    drawCmd->startRecord();
    drawCmd->beginRendering(*pipeline);
    drawCmd->setViewportAndScissor(glm::uvec2(currentResolution.x, currentResolution.y));
    drawCmd->drawIndirect(*indirectBuffer, descriptorSet.get(), vertexBuffer.get());
    drawCmd->endRendering();
    drawCmd->endRecord();
    drawCmd->runAsync();
    drawCmd->wait();
}

void VectorGraphicsAssetRenderer::initializeResources(Renderer& renderer)
{
    auto& device = renderer.getDevice();

    std::vector<uint32_t> vertSpirv(TextureQuad_vert_spirv, TextureQuad_vert_spirv + (sizeof(TextureQuad_vert_spirv) / sizeof(uint32_t)));
    std::vector<uint32_t> fragSpirv(TextureQuad_frag_spirv, TextureQuad_frag_spirv + (sizeof(TextureQuad_frag_spirv) / sizeof(uint32_t)));
    vertShader = device.createShader(vertSpirv);
    fragShader = device.createShader(fragSpirv);

    sceneDataUbo   = device.createUniform(1, sizeof(Vulcant::Toolkit::SceneData));
    quadParamsUbo  = device.createUniform(1, sizeof(AssetMeshParams));
    vertexBuffer   = device.createVertexBuffer(6, sizeof(Vulcant::Toolkit::MeshVertex), false);
    indirectBuffer = device.createIndirectBuffer(1, 16, false);
    drawCmd        = device.createGraphicCommand();

    uint32_t drawArgs[4] = { 6u, 1u, 0u, 0u };
    indirectBuffer->uploadToGPU(drawArgs, 1);
}

void VectorGraphicsAssetRenderer::uploadQuadVertices()
{
    const glm::dvec2& size  = asset->getWorldSize();
    float             halfW = static_cast<float>(size.x * 0.5);
    float             halfH = static_cast<float>(size.y * 0.5);
    glm::vec3         n(0.0f, 0.0f, 1.0f);

    std::vector<Vulcant::Toolkit::MeshVertex> vertices = {
        { glm::vec3(-halfW, -halfH, 0.0f), n, glm::vec3(0.0f, 1.0f, 0.0f) },
        {  glm::vec3(halfW, -halfH, 0.0f), n, glm::vec3(1.0f, 1.0f, 0.0f) },
        {  glm::vec3(halfW,  halfH, 0.0f), n, glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(-halfW, -halfH, 0.0f), n, glm::vec3(0.0f, 1.0f, 0.0f) },
        {  glm::vec3(halfW,  halfH, 0.0f), n, glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(-halfW,  halfH, 0.0f), n, glm::vec3(0.0f, 0.0f, 0.0f) }
    };

    vertexBuffer->uploadToGPU(vertices.data(), vertices.size(), 0);
}

void VectorGraphicsAssetRenderer::uploadRasterizedImage(Renderer& renderer)
{
    auto& device = renderer.getDevice();
    if (drawCmd)
    {
        drawCmd->wait();
    }

    constexpr uint32_t maxDimension = 1024;
    const glm::dvec2&  size         = asset->getWorldSize();
    uint32_t           rasterWidth  = maxDimension;
    uint32_t           rasterHeight = maxDimension;
    if (size.x > 1e-9 && size.y > 1e-9)
    {
        if (size.x >= size.y)
        {
            rasterWidth  = maxDimension;
            rasterHeight = std::max(1u, static_cast<uint32_t>(std::round(static_cast<double>(maxDimension) * (size.y / size.x))));
        }
        else
        {
            rasterHeight = maxDimension;
            rasterWidth  = std::max(1u, static_cast<uint32_t>(std::round(static_cast<double>(maxDimension) * (size.x / size.y))));
        }
    }

    CADacomb::Texture::Texture rasterized = CADacomb::VectorGraphics::VectorGraphicsRasterizer::rasterize(asset->getVectorGraphics(), rasterWidth, rasterHeight, glm::u8vec4(255, 255, 255, 255));

    descriptorSet.reset();
    textureImage = CADacombRender::TextureImageConverter::toVulcantImage(device, rasterized, Vulcant::VulcantImageUsage::SampleOnly);

    std::vector<std::shared_ptr<Vulcant::VulcantResource>> set0 = { std::shared_ptr<Vulcant::VulcantResource>(sceneDataUbo->asResource()),
                                                                    std::shared_ptr<Vulcant::VulcantResource>(quadParamsUbo->asResource()) };
    std::vector<std::shared_ptr<Vulcant::VulcantResource>> set1 = { std::shared_ptr<Vulcant::VulcantResource>(textureImage->asResource()) };

    descriptorSet = device.createSet({ set0, set1 }, *fragShader);
}

void VectorGraphicsAssetRenderer::rebuildPipeline(Renderer& renderer)
{
    auto& device = renderer.getDevice();
    if (drawCmd)
    {
        drawCmd->wait();
    }

    Vulcant::VulcantRasterizationConfig config;
    config.setCullMode(Vulcant::VulcantCullMode::None);
    config.setClear(false);

    pipeline = device.createVulcanGraphicPipeline({ fragShader.get(), vertShader.get() }, { &renderer.getColor(), &renderer.getNormal() }, &renderer.getDepth(), nullptr, config);
    drawCmd  = device.createGraphicCommand();
}
