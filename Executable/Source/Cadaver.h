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

class AboutWindow;
class AssetRenderFactory;
class CameraController;
class CommandsWindow;
class CreateMenu;
class FileMenu;
class EditMenu;
class LandscapeGeneratorWindow;
class PreferencesWindow;
class ProjectWindow;
class ViewCubeWidget;
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
    std::unique_ptr<EditMenu>                       editMenu;
    std::unique_ptr<CreateMenu>                     createMenu;
    std::unique_ptr<LandscapeGeneratorWindow>       landscapeGeneratorWindow;
    std::unique_ptr<ViewCubeWidget>                 viewCubeWidget;
    std::unique_ptr<CameraController>               cameraController;
    size_t                                          lastAssetCount = 0;
    glm::ivec2                                      resolution     = glm::ivec2(1280, 720);
    bool                                            quitRequested  = false;
};
