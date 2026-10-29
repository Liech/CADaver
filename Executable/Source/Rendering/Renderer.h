#pragma once

#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <memory>

class GatedDatabase;

namespace Vulcant
{
    class VulcantDevice;
    class VulcantGraphicCommand;
    class VulcantImage;
    class VulcantUi;
    class VulcantWindow;
}

// Coordinates rendering pipelines, offscreen target images, scene data, and ImGui UI rendering.
class Renderer
{
  public:
    Renderer(Vulcant::VulcantDevice& device, Vulcant::VulcantWindow& window, const glm::ivec2& resolution, GatedDatabase& gatedDatabase);
    virtual ~Renderer();

    void setResolution(const glm::ivec2& resolution);
    void setSceneData(const Vulcant::Toolkit::SceneData& sceneData);

    void clear();
    void beginUiFrame();
    void renderUi(Vulcant::VulcantWindow& window);

    const glm::ivec2&                             getResolution() const;
    const Vulcant::Toolkit::SceneData&            getSceneData() const;
    glm::vec4                                     getClearColor() const;
    Vulcant::Toolkit::MeshRenderer::WireframeMode getWireframeMode() const;
    glm::vec3                                     getWireframeColor() const;
    Vulcant::VulcantCullMode                      getCullMode() const;
    Vulcant::VulcantDevice&                       getDevice() const;
    Vulcant::VulcantImage&                        getColor() const;
    Vulcant::VulcantImage&                        getDepth() const;
    Vulcant::VulcantImage&                        getNormal() const;
    Vulcant::VulcantImage&                        getResult() const;

  private:
    void recordClearPass();
    void recordUiCommand();

    Vulcant::VulcantDevice&                         device;
    glm::ivec2                                      resolution;
    GatedDatabase&                                  gatedDatabase;
    Vulcant::Toolkit::SceneData                     sceneData;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> uiCmdG;
    std::unique_ptr<Vulcant::VulcantUi>             ui;
    std::unique_ptr<Vulcant::VulcantImage>          colorImage;
    std::unique_ptr<Vulcant::VulcantImage>          depthImage;
    std::unique_ptr<Vulcant::VulcantImage>          normalImage;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> clearRenderer;
};
