#pragma once

#include "AssetMeshParams.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace CADacomb::Animation
{
    class Skeleton;
}

namespace Vulcant
{
    class VulcantBuffer;
    class VulcantGraphicCommand;
    class VulcantGraphicPipeline;
    class VulcantImage;
    class VulcantSet;
    class VulcantShader;
}

class Renderer;

// Renders an X-ray 3D bone and joint overlay for a selected SkeletonComponent.
class SkeletonOverlayRenderer
{
  public:
    SkeletonOverlayRenderer();
    virtual ~SkeletonOverlayRenderer();

    void render(Renderer&                            renderer,
                const Vulcant::Toolkit::SceneData&   sceneData,
                const glm::dmat4&                    assetTransform,
                const CADacomb::Animation::Skeleton& skeleton,
                const std::vector<glm::dmat4>&       modelMatrices);

  private:
    void initializeResources(Renderer& renderer);
    void ensurePipeline(Renderer& renderer, const glm::ivec2& targetRes);
    void buildBoneGeometry(const CADacomb::Animation::Skeleton& skeleton, const std::vector<glm::dmat4>& modelMatrices);
    void appendTriangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& color);
    void appendBoneOctahedron(const glm::vec3& startPos, const glm::vec3& endPos, const glm::vec3& color);
    void appendJointOctahedron(const glm::vec3& center, float radius, const glm::vec3& color);

    std::unique_ptr<Vulcant::VulcantShader>          vertShader;
    std::unique_ptr<Vulcant::VulcantShader>          fragShader;
    std::unique_ptr<Vulcant::VulcantGraphicPipeline> pipeline;
    std::unique_ptr<Vulcant::VulcantSet>             descriptorSet;
    std::unique_ptr<Vulcant::VulcantBuffer>          sceneDataUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>          meshParamsUbo;
    std::unique_ptr<Vulcant::VulcantBuffer>          vertexBuffer;
    std::unique_ptr<Vulcant::VulcantBuffer>          indirectBuffer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>  drawCmd;
    std::vector<Vulcant::Toolkit::MeshVertex>        cachedVertices;
    AssetMeshParams                                  meshParams{};
    glm::ivec2                                       lastResolution       = glm::ivec2(0, 0);
    const Vulcant::VulcantImage*                     lastColorImage       = nullptr;
    uint32_t                                         vertexCount          = 0;
    uint32_t                                         vertexBufferCapacity = 0;
};
