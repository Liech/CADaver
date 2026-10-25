#pragma once

#include "CameraPreferencesView.h"
#include "RenderPreferencesView.h"
#include "SVOPreferencesView.h"
#include "UIPreferencesView.h"

namespace Vulcant::Toolkit
{
    class Camera;
}

class Preferences;

// Renders the preferences dialog window composed of UI, Camera, and Render tabs using ImGui.
class PreferencesWindow
{
  public:
    PreferencesWindow();
    virtual ~PreferencesWindow();

    void render(const Preferences& preferences, Vulcant::Toolkit::Camera* camera = nullptr);

    bool isOpen() const;
    void setOpen(bool open);

    bool isResetRequested() const;
    void clearResetRequested();

    UIPreferencesView&     getUIPreferencesView();
    CameraPreferencesView& getCameraPreferencesView();
    RenderPreferencesView& getRenderPreferencesView();
    SVOPreferencesView&    getSVOPreferencesView();

  private:
    UIPreferencesView     uiPreferencesView;
    CameraPreferencesView cameraPreferencesView;
    RenderPreferencesView renderPreferencesView;
    SVOPreferencesView    svoPreferencesView;
    bool                  openState      = false;
    bool                  resetRequested = false;
};
