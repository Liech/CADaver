#pragma once

#include "Commands/GatedDatabase.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Vulcant
{
    class VulcantDevice;
    class VulcantWindow;
    enum class VulcantInputValue : short;
}

class ProjectWindow;
class Renderer;
class TopMenu;
class ViewController;

// Main application class coordinating windowing, UI, renderer, project, and asset rendering.
class Cadaver
{
  public:
    Cadaver(Vulcant::VulcantDevice& device, const glm::ivec2& resolution = glm::ivec2(1280, 720));
    virtual ~Cadaver();

    static void run();

    void changeResolution(const glm::ivec2& newResolution);

    Vulcant::VulcantWindow& getWindow();
    bool                    isQuitRequested() const;

  private:
    void onLogic(double delta);
    void onRender();
    void onResize(const glm::ivec2& newResolution);

    void prepare();
    void renderImGui();

    void onKey(const Vulcant::VulcantInputValue& key);

    Vulcant::VulcantDevice&                 device;
    std::unique_ptr<Vulcant::VulcantWindow> window;
    std::unique_ptr<Renderer>               renderer;
    std::unique_ptr<ViewController>         viewController;
    std::unique_ptr<TopMenu>                topMenu;
    std::unique_ptr<ProjectWindow>          projectWindow;
    size_t                                  lastAssetCount = 0;
    glm::ivec2                              resolution;
    bool                                    quitRequested = false;
};
