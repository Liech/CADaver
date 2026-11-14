#pragma once

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <memory>

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
class ViewportPickingController;

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

    GatedDatabase& getGatedDatabase();
    Defer&         getDefer();

  private:
    void onLogic(double delta);
    void onRender();
    void onResize(const glm::ivec2& newResolution);

    void prepare();
    void renderImGui();

    void onKey(const Vulcant::VulcantInputValue& key);

    Vulcant::VulcantDevice&                    device;
    Defer                                      defer;
    GatedDatabase                              gatedDatabase;
    UndoStack                                  undoStack;
    Windows                                    windows;
    std::unique_ptr<Vulcant::VulcantWindow>    window;
    std::unique_ptr<Renderer>                  renderer;
    std::unique_ptr<ViewController>            viewController;
    std::unique_ptr<TopMenu>                   topMenu;
    std::unique_ptr<ProjectWindow>             projectWindow;
    std::unique_ptr<ViewportPickingController> pickingController;
    size_t                                     lastAssetCount = 0;
    glm::ivec2                                 resolution;
    bool                                       quitRequested = false;
};
