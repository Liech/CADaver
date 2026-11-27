#include "Rendering/TriangulationAssetRenderer.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Database/AssetComponent.h"
#include "Database/Component/AnimationComponent.h"
#include "Database/Component/SkeletonComponent.h"
#include "Database/Component/TextureComponent.h"
#include "Database/Component/TriangulationSkinningComponent.h"
#include "Database/Component/UVComponent.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Rendering/Renderer.h"
#include "Rendering/SkeletonOverlayRenderer.h"
#include "Rendering/Triangulation/DefaultSurfaceModule.h"
#include "Rendering/Triangulation/IndexedFaceColorModule.h"
#include "Rendering/Triangulation/TexturedSurfaceModule.h"
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
    surfaceModules.push_back(std::make_unique<TexturedSurfaceModule>());
}

TriangulationAssetRenderer::TriangulationAssetRenderer(std::shared_ptr<const TriangulationAsset> assetInput)
  : asset(std::move(assetInput))
{
    surfaceModules.push_back(std::make_unique<DefaultSurfaceModule>());
    surfaceModules.push_back(std::make_unique<IndexedFaceColorModule>());
    surfaceModules.push_back(std::make_unique<TexturedSurfaceModule>());
}

TriangulationAssetRenderer::~TriangulationAssetRenderer()
{
    if (drawCmd)
    {
        drawCmd->wait();
        drawCmd.reset();
    }
    skeletonOverlayRenderer.reset();
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

std::shared_ptr<AnimationComponent> TriangulationAssetRenderer::resolveSelectedAnimation(GatedDatabase& gatedDatabase) const
{
    if (!asset)
    {
        return nullptr;
    }

    auto skeletons  = asset->getAttachments<SkeletonComponent>();
    auto skinnings  = asset->getAttachments<TriangulationSkinningComponent>();
    auto animations = asset->getAttachments<AnimationComponent>();
    if (skeletons.empty() || skinnings.empty() || animations.empty())
    {
        return nullptr;
    }

    for (const auto& animComp : animations)
    {
        Polyglot::Node selRes = gatedDatabase.execute({
          {      "type",            "GetAssetSelection" },
          {   "version",                              1 },
          { "assetGuid", animComp->getGuid().toString() }
        });
        if (selRes.toJson().value("type", "") == "GetAssetSelection")
        {
            return animComp;
        }
    }

    return nullptr;
}

void TriangulationAssetRenderer::rebuildVertices()
{
    rebuildVerticesAtTime(nullptr, 0.0);
}

void TriangulationAssetRenderer::rebuildVerticesAtTime(const std::shared_ptr<AnimationComponent>& selectedAnimation, double animationTime)
{
    cachedVertices.clear();
    if (!asset || asset->getTriangulation().indices.empty())
    {
        vertexCount = 0;
        return;
    }

    const auto&                    triangulation   = asset->getTriangulation();
    const std::vector<glm::dvec3>* vertexPositions = &triangulation.vertices;
    std::vector<glm::dvec3>        skinnedPositions;

    if (selectedAnimation)
    {
        auto        skeletonComp = asset->getAttachments<SkeletonComponent>().front();
        auto        skinningComp = asset->getAttachments<TriangulationSkinningComponent>().front();
        const auto& skeleton     = skeletonComp->getSkeleton();
        const auto& skinning     = skinningComp->getSkinning();
        const auto& clip         = selectedAnimation->getClip();
        auto        pose         = clip.evaluate(skeleton, animationTime, selectedAnimation->isLooping());
        auto        skinMatrices = pose.computeSkinningMatrices(skeleton);
        skinnedPositions         = skinning.computeSkinnedPositions(triangulation.vertices, skinMatrices);
        vertexPositions          = &skinnedPositions;
    }

    lastColor             = asset->getMeshColor();
    glm::vec3 renderColor = lastSelected ? glm::mix(lastColor, glm::vec3(1.0f, 0.65f, 0.1f), 0.65f) : lastColor;
    cachedVertices.reserve(triangulation.indices.size());

    for (size_t i = 0; i + 2 < triangulation.indices.size(); i += 3)
    {
        size_t idx0 = triangulation.indices[i];
        size_t idx1 = triangulation.indices[i + 1];
        size_t idx2 = triangulation.indices[i + 2];

        if (idx0 >= vertexPositions->size() || idx1 >= vertexPositions->size() || idx2 >= vertexPositions->size())
        {
            continue;
        }

        glm::vec3 p0 = glm::vec3((*vertexPositions)[idx0]);
        glm::vec3 p1 = glm::vec3((*vertexPositions)[idx1]);
        glm::vec3 p2 = glm::vec3((*vertexPositions)[idx2]);

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
    meshParamsUbo                  = device.createUniform(1, sizeof(AssetMeshParams));
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
    outComponent      = nullptr;
    auto uvComponents = asset->getAttachments<UVComponent>();
    bool hasUVs       = !uvComponents.empty() && uvComponents.front()->getUVs().getCornerCount() > 0;

    for (const auto& component : asset->getAttachments())
    {
        Polyglot::Node compSel = gatedDatabase.execute({
          {      "type",             "GetAssetSelection" },
          {   "version",                               1 },
          { "assetGuid", component->getGuid().toString() }
        });
        if (compSel.toJson().value("type", "") == "GetAssetSelection")
        {
            if (component->getType() == "Texture" && !hasUVs)
            {
                continue;
            }
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

    if (hasUVs)
    {
        for (const auto& textureComponent : asset->getAttachments<TextureComponent>())
        {
            if (!textureComponent->getTexture().isEmpty())
            {
                for (const auto& module : surfaceModules)
                {
                    if (module->isCompatible(textureComponent->getType()))
                    {
                        outComponent = textureComponent.get();
                        return module.get();
                    }
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

    Polyglot::Node    renderPrefRes = gatedDatabase.execute({
      {    "type", "GetRenderPreferences" },
      { "version",                      1 }
    });
    RenderPreferences renderPreferences;
    if (renderPrefRes.contains("preferences"))
    {
        renderPreferences.fromNode(renderPrefRes["preferences"]);
    }

    const Vulcant::VulcantImage* targetColor = &renderer.getColor();
    glm::ivec2                   targetRes   = targetColor->getResolution();
    if (targetRes.x <= 0 || targetRes.y <= 0)
    {
        return;
    }

    Vulcant::VulcantCullMode targetCullMode = static_cast<Vulcant::VulcantCullMode>(renderPreferences.getCullMode());
    if (targetRes != lastResolution || targetColor != lastColorImage || targetCullMode != lastCullMode)
    {
        lastResolution = targetRes;
        lastColorImage = targetColor;
        lastCullMode   = targetCullMode;
        invalidatePipelines();
        drawCmd = renderer.getDevice().createGraphicCommand();
    }

    sceneDataUbo->uploadToGPU(&sceneData, 1);

    meshParams.params.wireframeColor = glm::vec4(renderPreferences.getWireframeColor(), 1.5f);
    meshParams.params.wireframeMode  = renderPreferences.getWireframeMode();
    meshParams.setTransformation(asset->getTransformation());
    meshParamsUbo->uploadToGPU(&meshParams, 1);

    Polyglot::Node selRes          = gatedDatabase.execute({
      {      "type",         "GetAssetSelection" },
      {   "version",                           1 },
      { "assetGuid", asset->getGuid().toString() }
    });
    bool           currentSelected = (selRes.toJson().value("type", "") == "GetAssetSelection");

    auto           selectedAnimation = resolveSelectedAnimation(gatedDatabase);
    CADacomb::GUID activeAnimGuid    = selectedAnimation ? selectedAnimation->getGuid() : CADacomb::GUID();
    double         currentTime       = 0.0;
    if (selectedAnimation)
    {
        Polyglot::Node timeRes = gatedDatabase.execute({
          {    "type", "GetCurrentTime" },
          { "version",                1 }
        });
        if (timeRes.contains("currentTime"))
        {
            currentTime = timeRes["currentTime"].asDouble();
        }
    }

    bool animationChanged = (activeAnimGuid != lastAnimationGuid) || (selectedAnimation && currentTime != lastAnimationTime);
    if (dirty || asset->getRevision() != lastRevision || lastColor != asset->getMeshColor() || currentSelected != lastSelected || animationChanged || !vertexBuffer)
    {
        lastColor         = asset->getMeshColor();
        lastSelected      = currentSelected;
        lastAnimationGuid = activeAnimGuid;
        lastAnimationTime = currentTime;
        rebuildVerticesAtTime(selectedAnimation, currentTime);
        if (vertexCount > 0)
        {
            if (drawCmd)
            {
                drawCmd->wait();
            }
            if (!vertexBuffer || vertexCount > vertexBufferCapacity)
            {
                vertexBufferCapacity = vertexCount;
                vertexBuffer         = renderer.getDevice().createVertexBuffer(vertexBufferCapacity, sizeof(Vulcant::Toolkit::MeshVertex), false);
            }
            vertexBuffer->uploadToGPU(cachedVertices.data(), vertexCount, 0);
            uint32_t indirectCmd[4] = { vertexCount, 1u, 0u, 0u };
            indirectBuffer->uploadToGPU(indirectCmd, 1);
        }
        else
        {
            vertexBuffer.reset();
            vertexBufferCapacity = 0;
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

    renderSelectedSkeleton(renderer, gatedDatabase, sceneData, selectedAnimation, currentTime);
}

void TriangulationAssetRenderer::renderSelectedSkeleton(Renderer&                                  renderer,
                                                        GatedDatabase&                             gatedDatabase,
                                                        const Vulcant::Toolkit::SceneData&         sceneData,
                                                        const std::shared_ptr<AnimationComponent>& selectedAnimation,
                                                        double                                     currentTime)
{
    auto skeletons = asset->getAttachments<SkeletonComponent>();
    if (skeletons.empty())
    {
        return;
    }

    for (const auto& skeletonComp : skeletons)
    {
        Polyglot::Node selRes = gatedDatabase.execute({
          {      "type",                "GetAssetSelection" },
          {   "version",                                  1 },
          { "assetGuid", skeletonComp->getGuid().toString() }
        });
        if (selRes.toJson().value("type", "") != "GetAssetSelection")
        {
            continue;
        }

        const auto& skeleton = skeletonComp->getSkeleton();
        auto        pose     = skeleton.createRestPose();

        if (selectedAnimation)
        {
            const auto& clip = selectedAnimation->getClip();
            pose             = clip.evaluate(skeleton, currentTime, selectedAnimation->isLooping());
        }

        if (!skeletonOverlayRenderer)
        {
            skeletonOverlayRenderer = std::make_unique<SkeletonOverlayRenderer>();
        }

        auto modelMatrices = pose.computeModelMatrices(skeleton);
        skeletonOverlayRenderer->render(renderer, sceneData, asset->getTransformation(), skeleton, modelMatrices);
        return;
    }
}
