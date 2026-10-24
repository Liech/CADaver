#pragma once

#include "PreferencesData.h"
#include "Project/Project.h"

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
    class ArcCam;
    class MeshRenderer;
}

class FileMenu;
class PreferencesWindow;
class ProjectWindow;

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
    void frameCameraOnProject();

    Vulcant::VulcantDevice&                         device;
    std::unique_ptr<Vulcant::VulcantWindow>         window;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> uiCmdG;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    std::unique_ptr<Vulcant::VulcantUi>             ui;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> meshRenderer;
    PreferencesData                                 preferencesData;
    std::unique_ptr<PreferencesWindow>              preferencesWindow;
    Project                                         project;
    std::unique_ptr<ProjectWindow>                  projectWindow;
    std::unique_ptr<FileMenu>                       fileMenu;
    std::unique_ptr<Vulcant::Toolkit::ArcCam>       camera;
    glm::vec4                                       backgroundColor = glm::vec4(0.12f, 0.12f, 0.14f, 1.0f);
    size_t                                          lastAssetCount  = 0;
    glm::ivec2                                      resolution      = glm::ivec2(1280, 720);
    bool                                            quitRequested   = false;
};
