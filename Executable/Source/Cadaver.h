#pragma once
#include "PreferencesData.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Vulcant
{
    class VulcantDevice;
    class VulcantWindow;
    class VulcantImage;
    class VulcantGraphicCommand;
    class VulcantComputeCommand;
    class VulcantUi;
}

class PreferencesWindow;

class Cadaver
{
  public:
    Cadaver();
    virtual ~Cadaver();

    static void run();

    void createWindow(Vulcant::VulcantDevice& deviceInput, const glm::ivec2& resolution);
    void changeResolution(const glm::ivec2& newResolution);

    Vulcant::VulcantWindow& getWindow();
    bool                    isQuitRequested() const;

  private:
    void onLogic(double delta);
    void onRender();
    void onResize(const glm::ivec2& newResolution);

    void prepare(Vulcant::VulcantDevice& deviceInput, const glm::ivec2& resolution);
    void recordCommand();
    void renderImGui();

    Vulcant::VulcantDevice*                         device = nullptr;
    std::unique_ptr<Vulcant::VulcantWindow>         window;
    std::unique_ptr<Vulcant::VulcantImage>          renderImage;
    std::unique_ptr<Vulcant::VulcantComputeCommand> computeCmd;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> uiCmdG;
    std::unique_ptr<Vulcant::VulcantUi>             ui;
    PreferencesData                                 preferencesData;
    std::unique_ptr<PreferencesWindow>              preferencesWindow;
    glm::ivec2                                      resolution    = glm::ivec2(1280, 720);
    bool                                            quitRequested = false;
};
