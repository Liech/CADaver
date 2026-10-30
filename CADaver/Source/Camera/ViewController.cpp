#include "Camera/ViewController.h"

#include "Camera/CameraController.h"
#include "UI/Component/ViewCubeWidget.h"

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

void ViewController::renderViewCube(const glm::ivec2& resolution, const Project& project)
{
    if (viewCubeWidget && cameraController && cameraController->getCamera())
    {
        viewCubeWidget->render(cameraController->getCamera(),
                               resolution,
                               [this, &project]()
                               {
                                   if (cameraController)
                                   {
                                       cameraController->frameOnProject(project);
                                   }
                               });
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewController/InitialState")
{
    ViewController controller;
    CHECK(controller.getCamera() == nullptr);
    CHECK(controller.getMode() == -1);
}
#endif
