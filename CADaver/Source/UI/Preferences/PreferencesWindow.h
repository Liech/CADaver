#pragma once

#include "CameraPreferencesView.h"
#include "RenderPreferencesView.h"
#include "SVOPreferencesView.h"
#include "UIPreferencesView.h"
#include "UndoPreferencesView.h"

#include <string>

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

    void load(const std::string& filepath = "preferences.json");
    void save(const Preferences& preferences, const std::string& filepath = "preferences.json") const;

    bool isOpen() const;
    void setOpen(bool open);

    bool isResetRequested() const;
    void clearResetRequested();

    UIPreferencesView&     getUIPreferencesView();
    CameraPreferencesView& getCameraPreferencesView();
    RenderPreferencesView& getRenderPreferencesView();
    SVOPreferencesView&    getSVOPreferencesView();
    UndoPreferencesView&   getUndoPreferencesView();

  private:
    UIPreferencesView     uiPreferencesView;
    CameraPreferencesView cameraPreferencesView;
    RenderPreferencesView renderPreferencesView;
    SVOPreferencesView    svoPreferencesView;
    UndoPreferencesView   undoPreferencesView;
    bool                  openState      = false;
    bool                  resetRequested = false;
};
