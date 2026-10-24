#pragma once

namespace Vulcant::Toolkit
{
    class Camera;
}

class CameraPreferences;

// Renders camera controls, speed, look sensitivity, and scene frustum settings using ImGui.
class CameraPreferencesView
{
  public:
    CameraPreferencesView();
    virtual ~CameraPreferencesView();

    bool render(CameraPreferences& preferences, Vulcant::Toolkit::Camera* camera = nullptr);
};
