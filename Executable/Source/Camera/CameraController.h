#pragma once

#include "Vulcant/Toolkit/SceneData.h"

#include <glm/glm.hpp>
#include <memory>

namespace Vulcant
{
    class VulcantWindow;
    enum class VulcantInputValue : short;
}

namespace Vulcant::Toolkit
{
    class Camera;
}

class CameraPreferences;
class Project;

// Manages viewport camera lifecycle, mode switching, input events, and scene framing.
class CameraController
{
  public:
    CameraController();
    virtual ~CameraController();

    void setMode(int mode, Vulcant::VulcantWindow& window, const CameraPreferences& camPrefs, const glm::ivec2& resolution);
    int  getMode() const;

    void updateAspect(const glm::ivec2& resolution);
    void tick(double delta);
    void keyEvent(const Vulcant::VulcantInputValue& key);

    void frameOnProject(const Project& project);
    void reset(const CameraPreferences& camPrefs, const Project& project);

    bool isOrthographic() const;
    void setOrthographic(bool ortho);
    void toggleOrthographic();

    Vulcant::Toolkit::Camera*   getCamera() const;
    Vulcant::Toolkit::SceneData getSceneData() const;

  private:
    std::unique_ptr<Vulcant::Toolkit::Camera> camera;
    int                                       currentCameraMode = -1;
};
