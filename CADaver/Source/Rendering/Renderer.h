#pragma once

#include "CADacomb/Util/GUID.h"
#include "Vulcant/Interface/VulcantCullMode.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <vector>

class Asset;
class AssetRenderer;
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

    void render(const Vulcant::Toolkit::SceneData& sceneData);
    void beginUiFrame();

    Vulcant::VulcantDevice& getDevice() const;
    Vulcant::VulcantImage&  getColor() const;
    Vulcant::VulcantImage&  getDepth() const;
    Vulcant::VulcantImage&  getNormal() const;
    Vulcant::VulcantImage&  getResult() const;

  private:
    glm::vec4 getClearColor() const;
    void      clear();
    void      renderAsset(const std::shared_ptr<const Asset>& asset, const Vulcant::Toolkit::SceneData& sceneData);
    void      pruneRenderers(const std::vector<std::shared_ptr<const Asset>>& activeAssets);
    void      renderUi();
    void      recordClearPass();
    void      recordUiCommand();

    Vulcant::VulcantDevice&                                  device;
    Vulcant::VulcantWindow&                                  window;
    glm::ivec2                                               resolution;
    GatedDatabase&                                           gatedDatabase;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>          drawCmd;
    std::unique_ptr<Vulcant::VulcantGraphicCommand>          uiCmdG;
    std::unique_ptr<Vulcant::VulcantUi>                      ui;
    std::unique_ptr<Vulcant::VulcantImage>                   colorImage;
    std::unique_ptr<Vulcant::VulcantImage>                   depthImage;
    std::unique_ptr<Vulcant::VulcantImage>                   normalImage;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer>          clearRenderer;
    std::map<CADacomb::GUID, std::shared_ptr<AssetRenderer>> renderers;
};
