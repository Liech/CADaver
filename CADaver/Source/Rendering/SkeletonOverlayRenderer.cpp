#include "Rendering/SkeletonOverlayRenderer.h"

#include "CADacomb/Animation/Skeleton/Skeleton.h"
#include "Rendering/Renderer.h"
#include "Triangulation/Triangulation_Default_frag.h"
#include "Triangulation/Triangulation_vert.h"
#include "Vulcant/Interface/VulcantBuffer.h"
#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantGraphicPipeline.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantRasterizationConfig.h"
#include "Vulcant/Interface/VulcantResource.h"
#include "Vulcant/Interface/VulcantSet.h"
#include "Vulcant/Interface/VulcantShader.h"

#include <algorithm>
#include <cmath>

SkeletonOverlayRenderer::SkeletonOverlayRenderer() {}

SkeletonOverlayRenderer::~SkeletonOverlayRenderer()
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
}

void SkeletonOverlayRenderer::initializeResources(Renderer& renderer)
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

void SkeletonOverlayRenderer::ensurePipeline(Renderer& renderer, const glm::ivec2& targetRes)
{
    if (targetRes != lastResolution)
    {
        lastResolution = targetRes;
        descriptorSet.reset();
        pipeline.reset();
        if (targetRes.x > 0 && targetRes.y > 0)
        {
            drawCmd = renderer.getDevice().createGraphicCommand();
        }
    }

    Vulcant::VulcantDevice& device = renderer.getDevice();
    if (!pipeline)
    {
        std::vector<Vulcant::VulcantShader*> shaders          = { fragShader.get(), vertShader.get() };
        std::vector<Vulcant::VulcantImage*>  colorAttachments = { &renderer.getColor(), &renderer.getNormal() };

        Vulcant::VulcantRasterizationConfig rasterConfig;
        rasterConfig.setCullMode(Vulcant::VulcantCullMode::None);
        rasterConfig.setClear(false);

        pipeline = device.createVulcanGraphicPipeline(shaders, colorAttachments, nullptr, nullptr, rasterConfig);
    }

    if (!descriptorSet)
    {
        std::vector<std::vector<std::shared_ptr<Vulcant::VulcantResource>>> sets;
        sets.push_back({ sceneDataUbo->asResource(), meshParamsUbo->asResource() });
        descriptorSet = device.createSet(sets, *fragShader);
    }
}

void SkeletonOverlayRenderer::appendTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color)
{
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

    cachedVertices.push_back({ p0, normal, color });
    cachedVertices.push_back({ p1, normal, color });
    cachedVertices.push_back({ p2, normal, color });
}

void SkeletonOverlayRenderer::appendBoneOctahedron(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec3& color)
{
    glm::vec3 delta  = endPos - startPos;
    float     length = glm::length(delta);
    if (length <= 1e-4f)
    {
        return;
    }

    glm::vec3 dir    = delta / length;
    glm::vec3 upHint = std::abs(dir.y) < 0.9f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 sideA  = glm::normalize(glm::cross(dir, upHint));
    glm::vec3 sideB  = glm::normalize(glm::cross(dir, sideA));

    float     radius = std::clamp(length * 0.1f, 0.003f, 0.03f);
    glm::vec3 mid    = startPos + dir * (length * 0.2f);
    glm::vec3 r0     = mid + sideA * radius;
    glm::vec3 r1     = mid + sideB * radius;
    glm::vec3 r2     = mid - sideA * radius;
    glm::vec3 r3     = mid - sideB * radius;

    appendTriangle(startPos, r0, r1, color);
    appendTriangle(startPos, r1, r2, color);
    appendTriangle(startPos, r2, r3, color);
    appendTriangle(startPos, r3, r0, color);

    appendTriangle(endPos, r1, r0, color);
    appendTriangle(endPos, r2, r1, color);
    appendTriangle(endPos, r3, r2, color);
    appendTriangle(endPos, r0, r3, color);
}

void SkeletonOverlayRenderer::appendJointOctahedron(const glm::vec3& center, float radius, const glm::vec3& color)
{
    glm::vec3 px = center + glm::vec3(radius, 0.0f, 0.0f);
    glm::vec3 nx = center - glm::vec3(radius, 0.0f, 0.0f);
    glm::vec3 py = center + glm::vec3(0.0f, radius, 0.0f);
    glm::vec3 ny = center - glm::vec3(0.0f, radius, 0.0f);
    glm::vec3 pz = center + glm::vec3(0.0f, 0.0f, radius);
    glm::vec3 nz = center - glm::vec3(0.0f, 0.0f, radius);

    appendTriangle(py, px, pz, color);
    appendTriangle(py, pz, nx, color);
    appendTriangle(py, nx, nz, color);
    appendTriangle(py, nz, px, color);

    appendTriangle(ny, pz, px, color);
    appendTriangle(ny, nx, pz, color);
    appendTriangle(ny, nz, nx, color);
    appendTriangle(ny, px, nz, color);
}

void SkeletonOverlayRenderer::buildBoneGeometry(const CADacomb::Animation::Skeleton& skeleton, const std::vector<glm::dmat4>& modelMatrices)
{
    cachedVertices.clear();
    size_t jointCount = std::min(skeleton.getJointCount(), modelMatrices.size());
    if (jointCount == 0)
    {
        vertexCount = 0;
        return;
    }

    float  boneLengthSum = 0.0f;
    size_t boneCount     = 0;
    for (size_t i = 0; i < jointCount; i++)
    {
        int parentIdx = skeleton.getJoint(i).getParentIndex();
        if (parentIdx >= 0 && static_cast<size_t>(parentIdx) < jointCount)
        {
            glm::vec3 p0  = glm::vec3(modelMatrices[static_cast<size_t>(parentIdx)][3]);
            glm::vec3 p1  = glm::vec3(modelMatrices[i][3]);
            float     len = glm::length(p1 - p0);
            if (len > 1e-4f)
            {
                boneLengthSum += len;
                boneCount++;
            }
        }
    }

    float     avgBoneLen  = boneCount > 0 ? boneLengthSum / static_cast<float>(boneCount) : 0.1f;
    float     jointRadius = std::clamp(avgBoneLen * 0.08f, 0.004f, 0.018f);
    glm::vec3 boneColor(0.25f, 0.85f, 1.0f);
    glm::vec3 jointColor(1.0f, 0.72f, 0.15f);

    cachedVertices.reserve(jointCount * 48);

    for (size_t i = 0; i < jointCount; i++)
    {
        glm::vec3 jointPos  = glm::vec3(modelMatrices[i][3]);
        int       parentIdx = skeleton.getJoint(i).getParentIndex();
        if (parentIdx >= 0 && static_cast<size_t>(parentIdx) < jointCount)
        {
            glm::vec3 parentPos = glm::vec3(modelMatrices[static_cast<size_t>(parentIdx)][3]);
            appendBoneOctahedron(parentPos, jointPos, boneColor);
        }
        appendJointOctahedron(jointPos, jointRadius, jointColor);
    }

    vertexCount = static_cast<uint32_t>(cachedVertices.size());
}

void SkeletonOverlayRenderer::render(Renderer&                            renderer,
                                     const Vulcant::Toolkit::SceneData&   sceneData,
                                     const glm::dmat4&                    assetTransform,
                                     const CADacomb::Animation::Skeleton& skeleton,
                                     const std::vector<glm::dmat4>&       modelMatrices)
{
    if (!vertShader)
    {
        initializeResources(renderer);
    }

    glm::ivec2 targetRes = renderer.getColor().getResolution();
    if (targetRes.x <= 0 || targetRes.y <= 0)
    {
        return;
    }

    ensurePipeline(renderer, targetRes);
    buildBoneGeometry(skeleton, modelMatrices);

    if (vertexCount == 0 || !drawCmd)
    {
        return;
    }

    drawCmd->wait();
    if (!vertexBuffer || vertexCount > vertexBufferCapacity)
    {
        vertexBufferCapacity = vertexCount;
        vertexBuffer         = renderer.getDevice().createVertexBuffer(vertexBufferCapacity, sizeof(Vulcant::Toolkit::MeshVertex), false);
    }
    vertexBuffer->uploadToGPU(cachedVertices.data(), vertexCount, 0);

    uint32_t indirectCmd[4] = { vertexCount, 1u, 0u, 0u };
    indirectBuffer->uploadToGPU(indirectCmd, 1);

    sceneDataUbo->uploadToGPU(&sceneData, 1);

    meshParams.params.wireframeColor = glm::vec4(0.05f, 0.15f, 0.25f, 1.0f);
    meshParams.params.wireframeMode  = 1;
    meshParams.setTransformation(assetTransform);
    meshParamsUbo->uploadToGPU(&meshParams, 1);

    drawCmd->startRecord();
    drawCmd->beginRendering(*pipeline);
    drawCmd->setViewportAndScissor(glm::uvec2(targetRes.x, targetRes.y));
    drawCmd->drawIndirect(*indirectBuffer, descriptorSet.get(), vertexBuffer.get());
    drawCmd->endRendering();
    drawCmd->endRecord();
    drawCmd->runAsync();
    drawCmd->wait();
}
