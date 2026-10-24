#pragma once

#include "CameraPreferencesView.h"
#include "RenderPreferencesView.h"
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

    void render(Preferences& preferences, Vulcant::Toolkit::Camera* camera = nullptr);

    bool isOpen() const;
    void setOpen(bool open);

    UIPreferencesView&     getUIPreferencesView();
    CameraPreferencesView& getCameraPreferencesView();
    RenderPreferencesView& getRenderPreferencesView();

  private:
    UIPreferencesView     uiPreferencesView;
    CameraPreferencesView cameraPreferencesView;
    RenderPreferencesView renderPreferencesView;
    bool                  openState = false;
};
