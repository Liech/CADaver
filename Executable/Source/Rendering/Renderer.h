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
}

// Coordinates target images, scene data, and frame clear using database preferences.
class Renderer
{
  public:
    Renderer(Vulcant::VulcantDevice& device, const glm::ivec2& resolution, GatedDatabase& gatedDatabase);
    virtual ~Renderer();

    void setResolution(const glm::ivec2& resolution);
    void setSceneData(const Vulcant::Toolkit::SceneData& sceneData);

    void clear();

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

    Vulcant::VulcantDevice&                         device;
    glm::ivec2                                      resolution;
    GatedDatabase&                                  gatedDatabase;
    Vulcant::Toolkit::SceneData                     sceneData;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::unique_ptr<Vulcant::VulcantImage>          colorImage;
    std::unique_ptr<Vulcant::VulcantImage>          depthImage;
    std::unique_ptr<Vulcant::VulcantImage>          normalImage;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> clearRenderer;
};
