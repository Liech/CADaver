#pragma once

#include "Vulcant/Toolkit/SceneData.h"

#include <functional>
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
class GatedDatabase;
class Project;
class UIElementFactory;
class UndoStack;
class ViewCubeWidget;
class Windows;

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

    void rollCamera(double angle);
    void rotateAroundTarget(double angle, const glm::dvec3& axis);
    void rotateUp();
    void rotateDown();
    void rotateLeft();
    void rotateRight();
    void rollCcw();
    void rollCw();
    void toggleProjection();
    void resetView(const Project& project);

    void registerUIElements(UIElementFactory& factory, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);

    void renderViewCube(const glm::ivec2& resolution, const Project& project, const std::function<void(bool)>& onOrthographicChanged = nullptr);

  private:
    std::unique_ptr<CameraController> cameraController;
    std::unique_ptr<ViewCubeWidget>   viewCubeWidget;
};
