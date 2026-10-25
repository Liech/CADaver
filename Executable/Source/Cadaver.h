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
    class VulcantGraphicCommand;
    class VulcantUi;
    enum class VulcantInputValue : short;
}

namespace Vulcant::Toolkit
{
    class Camera;
}

class AboutWindow;
class AssetRenderFactory;
class CommandsWindow;
class FileMenu;
class PreferencesWindow;
class ProjectWindow;
class Renderer;

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
    void recordCommand();
    void renderImGui();

    void onKey(const Vulcant::VulcantInputValue& key);
    void setCameraMode(int mode);
    void resetCamera();
    void frameCameraOnProject();

    Vulcant::VulcantDevice&                         device;
    std::unique_ptr<Vulcant::VulcantWindow>         window;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> uiCmdG;
    std::unique_ptr<Vulcant::VulcantUi>             ui;
    std::unique_ptr<Renderer>                       renderer;
    std::unique_ptr<AssetRenderFactory>             assetRenderFactory;
    std::unique_ptr<PreferencesWindow>              preferencesWindow;
    std::unique_ptr<AboutWindow>                    aboutWindow;
    std::unique_ptr<CommandsWindow>                 commandsWindow;
    std::unique_ptr<ProjectWindow>                  projectWindow;
    std::unique_ptr<FileMenu>                       fileMenu;
    std::unique_ptr<Vulcant::Toolkit::Camera>       camera;
    size_t                                          lastAssetCount    = 0;
    int                                             currentCameraMode = -1;
    glm::ivec2                                      resolution        = glm::ivec2(1280, 720);
    bool                                            quitRequested     = false;
};
