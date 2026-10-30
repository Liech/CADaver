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

class CameraController;
class CameraPreferences;
class Project;
class ViewCubeWidget;

// Coordinates camera navigation, projection updates, user input events, and the ViewCube widget.
class ViewController
{
  public:
    ViewController();
    virtual ~ViewController();

    void setMode(int mode, Vulcant::VulcantWindow& window, const CameraPreferences& cameraPreferences, const glm::ivec2& resolution);
    int  getMode() const;

    void updateAspect(const glm::ivec2& resolution);
    void tick(double delta);
    void keyEvent(const Vulcant::VulcantInputValue& key);

    void frameOnProject(const Project& project);
    void reset(const CameraPreferences& cameraPreferences, const Project& project);

    Vulcant::Toolkit::Camera*   getCamera() const;
    Vulcant::Toolkit::SceneData getSceneData() const;
    CameraController&           getCameraController();

    void renderViewCube(const glm::ivec2& resolution, const Project& project);

  private:
    std::unique_ptr<CameraController> cameraController;
    std::unique_ptr<ViewCubeWidget>   viewCubeWidget;
};
