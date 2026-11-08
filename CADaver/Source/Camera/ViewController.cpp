#include "Camera/ViewController.h"

#include "Camera/CameraController.h"
#include "UI/Component/ViewCubeWidget.h"
#include "UI/ViewCube/ViewCubeHomeMenuEntry.h"
#include "UI/ViewCube/ViewCubeRollClockwiseMenuEntry.h"
#include "UI/ViewCube/ViewCubeRollCounterClockwiseMenuEntry.h"
#include "UI/ViewCube/ViewCubeRotateDownMenuEntry.h"
#include "UI/ViewCube/ViewCubeRotateLeftMenuEntry.h"
#include "UI/ViewCube/ViewCubeRotateRightMenuEntry.h"
#include "UI/ViewCube/ViewCubeRotateUpMenuEntry.h"
#include "UI/ViewCube/ViewCubeToggleOrthographicMenuEntry.h"
#include "UIElement/UIElementFactory.h"
#include "Vulcant/Toolkit/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

ViewController::ViewController()
{
    cameraController = std::make_unique<CameraController>();
    viewCubeWidget   = std::make_unique<ViewCubeWidget>();
}

ViewController::~ViewController() {}

void ViewController::setMode(int mode, Vulcant::VulcantWindow& window, const CameraPreferences& cameraPreferences, const glm::ivec2& resolution)
{
    if (cameraController)
    {
        cameraController->setMode(mode, window, cameraPreferences, resolution);
    }
}

int ViewController::getMode() const
{
    return cameraController ? cameraController->getMode() : -1;
}

void ViewController::updateAspect(const glm::ivec2& resolution)
{
    if (cameraController)
    {
        cameraController->updateAspect(resolution);
    }
}

void ViewController::tick(double delta)
{
    if (cameraController)
    {
        cameraController->tick(delta);
    }
}

void ViewController::keyEvent(const Vulcant::VulcantInputValue& key)
{
    if (cameraController)
    {
        cameraController->keyEvent(key);
    }
}

void ViewController::frameOnProject(const Project& project)
{
    if (cameraController)
    {
        cameraController->frameOnProject(project);
    }
}

void ViewController::reset(const CameraPreferences& cameraPreferences, const Project& project)
{
    if (cameraController)
    {
        cameraController->reset(cameraPreferences, project);
    }
}

Vulcant::Toolkit::Camera* ViewController::getCamera() const
{
    return cameraController ? cameraController->getCamera() : nullptr;
}

Vulcant::Toolkit::SceneData ViewController::getSceneData() const
{
    return cameraController ? cameraController->getSceneData() : Vulcant::Toolkit::SceneData{};
}

CameraController& ViewController::getCameraController()
{
    return *cameraController;
}

void ViewController::rollCamera(double angle)
{
    auto* camera = getCamera();
    if (!camera)
    {
        return;
    }

    glm::dvec3 eye     = camera->getEye();
    glm::dvec3 target  = camera->getTarget();
    glm::dvec3 forward = target - eye;
    if (glm::length(forward) < 1e-6)
    {
        return;
    }
    forward = glm::normalize(forward);

    glm::dmat4 rot   = glm::rotate(glm::dmat4(1.0), angle, forward);
    glm::dvec3 newUp = glm::normalize(glm::dvec3(rot * glm::dvec4(camera->getUp(), 0.0)));
    camera->setUp(newUp);
}

void ViewController::rotateAroundTarget(double angle, const glm::dvec3& axis)
{
    auto* camera = getCamera();
    if (!camera)
    {
        return;
    }

    glm::dvec3 eye    = camera->getEye();
    glm::dvec3 target = camera->getTarget();
    glm::dvec3 up     = camera->getUp();

    glm::dmat4 rot    = glm::rotate(glm::dmat4(1.0), angle, glm::normalize(axis));
    glm::dvec3 offset = eye - target;
    offset            = glm::dvec3(rot * glm::dvec4(offset, 0.0));
    up                = glm::dvec3(rot * glm::dvec4(up, 0.0));

    camera->setEye(target + offset);
    camera->setUp(up);
}

void ViewController::rotateUp()
{
    auto* camera = getCamera();
    if (!camera)
    {
        return;
    }
    glm::dvec3 forward = glm::normalize(camera->getTarget() - camera->getEye());
    glm::dvec3 right   = glm::normalize(glm::cross(forward, camera->getUp()));
    rotateAroundTarget(glm::radians(90.0), right);
}

void ViewController::rotateDown()
{
    auto* camera = getCamera();
    if (!camera)
    {
        return;
    }
    glm::dvec3 forward = glm::normalize(camera->getTarget() - camera->getEye());
    glm::dvec3 right   = glm::normalize(glm::cross(forward, camera->getUp()));
    rotateAroundTarget(-glm::radians(90.0), right);
}

void ViewController::rotateLeft()
{
    auto* camera = getCamera();
    if (!camera)
    {
        return;
    }
    rotateAroundTarget(glm::radians(90.0), camera->getUp());
}

void ViewController::rotateRight()
{
    auto* camera = getCamera();
    if (!camera)
    {
        return;
    }
    rotateAroundTarget(-glm::radians(90.0), camera->getUp());
}

void ViewController::rollCcw()
{
    rollCamera(glm::radians(90.0));
}

void ViewController::rollCw()
{
    rollCamera(-glm::radians(90.0));
}

void ViewController::toggleProjection()
{
    auto* camera = getCamera();
    if (camera)
    {
        camera->setOrthographic(!camera->isOrthographic());
    }
}

void ViewController::resetView(const Project& project)
{
    frameOnProject(project);
}

void ViewController::registerUIElements(UIElementFactory& factory, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
{
    factory.registerElement("ViewHome", [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeHomeMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewRollCounterClockwise",
                            [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeRollCounterClockwiseMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewRollClockwise", [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeRollClockwiseMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewToggleOrthographic",
                            [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeToggleOrthographicMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewRotateUp", [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeRotateUpMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewRotateDown", [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeRotateDownMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewRotateLeft", [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeRotateLeftMenuEntry>(*this, gatedDatabase, undoStack, windows); });
    factory.registerElement("ViewRotateRight", [this, &gatedDatabase, &undoStack, &windows]() { return std::make_shared<ViewCubeRotateRightMenuEntry>(*this, gatedDatabase, undoStack, windows); });
}

void ViewController::renderViewCube(const glm::ivec2& resolution, const Project& project, const std::function<void(bool)>& onOrthographicChanged)
{
    if (viewCubeWidget && cameraController && cameraController->getCamera())
    {
        viewCubeWidget->render(
          cameraController->getCamera(),
          resolution,
          [this, &project]()
          {
              if (cameraController)
              {
                  cameraController->frameOnProject(project);
              }
          },
          onOrthographicChanged);
    }
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewController/InitialState")
{
    ViewController controller;
    CHECK(controller.getCamera() == nullptr);
    CHECK(controller.getMode() == -1);
}

TEST_CASE("ViewController/RegisterUIElements")
{
    ViewController   controller;
    UIElementFactory factory;
    GatedDatabase    db;
    UndoStack        us(db);
    Windows          windows;

    controller.registerUIElements(factory, db, us, windows);

    REQUIRE(factory.create("ViewHome") != nullptr);
    REQUIRE(factory.create("ViewRollCounterClockwise") != nullptr);
    REQUIRE(factory.create("ViewRollClockwise") != nullptr);
    REQUIRE(factory.create("ViewToggleOrthographic") != nullptr);
    REQUIRE(factory.create("ViewRotateUp") != nullptr);
    REQUIRE(factory.create("ViewRotateDown") != nullptr);
    REQUIRE(factory.create("ViewRotateLeft") != nullptr);
    REQUIRE(factory.create("ViewRotateRight") != nullptr);
}
#endif
