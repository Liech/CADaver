#pragma once

#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Vulcant
{
    class VulcantDevice;
    class VulcantGraphicCommand;
    class VulcantImage;
}

namespace Vulcant::Toolkit
{
    class MeshRenderer;
    struct MeshVertex;
}

class Renderer
{
  public:
    Renderer(Vulcant::VulcantDevice& device, const glm::ivec2& resolution);
    virtual ~Renderer();

    void setResolution(const glm::ivec2& resolution);
    void setSceneData(const Vulcant::Toolkit::SceneData& sceneData);
    void setClearColor(const glm::vec4& color);
    void setWireframeMode(Vulcant::Toolkit::MeshRenderer::WireframeMode mode);
    void setWireframeColor(const glm::vec3& color);
    void setCullMode(Vulcant::VulcantCullMode mode);

    void beginRecord();
    void drawVertices(const std::vector<Vulcant::Toolkit::MeshVertex>& vertices);
    void drawVertices(const std::vector<Vulcant::Toolkit::MeshVertex>& vertices, const glm::vec3& colorOverride);
    void endRecord();

    void execute();

    bool needsRecord() const;
    void markDirty();

    Vulcant::VulcantGraphicCommand* getCommand() const;
    Vulcant::VulcantDevice&         getDevice() const;
    Vulcant::VulcantImage&          getResult() const;

  private:
    Vulcant::VulcantDevice&                         device;
    glm::ivec2                                      resolution;
    glm::vec4                                       clearColor     = glm::vec4(0.12f, 0.12f, 0.14f, 1.0f);
    glm::vec3                                       wireframeColor = glm::vec3(1.0f, 0.0f, 0.0f);
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> meshRenderer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::vector<Vulcant::Toolkit::MeshVertex>       accumulatedVertices;
    Vulcant::Toolkit::MeshRenderer::WireframeMode   wireframeMode = Vulcant::Toolkit::MeshRenderer::WireframeMode::Off;
    Vulcant::VulcantCullMode                        cullMode      = Vulcant::VulcantCullMode::Front;
    bool                                            dirty         = true;
};
