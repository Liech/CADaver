#include "Cadaver.h"

#include "Camera/ViewController.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Picking/ViewportPickingController.h"
#include "Rendering/Renderer.h"
#include "UI/Menu/TopMenu.h"
#include "UI/Project/ProjectWindow.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantInput.h"
#include "Vulcant/Interface/VulcantInputValue.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Vulcant/Toolkit/SceneData.h"
#include "Vulcant/VulcantV/VulcantVDevice.h"
#include "Vulcant/Wrapper/Window.h"
#include "Windows/Preferences/PreferencesWindow.h"

#include <imgui.h>

void Cadaver::run()
{
    auto                              windowExtensions = Vulcant::Wrapper::Window::getVulkanExtensions();
    Vulcant::VulcantV::VulcantVDevice device(windowExtensions);
    {
        Cadaver app(device, glm::ivec2(1280, 720));
        auto&   window = app.getWindow();

        while (!window.isClosed() && !app.isQuitRequested())
        {
            window.tick();
        }
    }
}

Cadaver::Cadaver(Vulcant::VulcantDevice& deviceInput, const glm::ivec2& res)
  : device(deviceInput)
  , undoStack(gatedDatabase)
  , resolution(res)
{
    window = device.createWindow(resolution, "CADaver");

    prepare();

    window->start([this](double delta) { onLogic(delta); }, [this]() { onRender(); }, [this](const glm::ivec2& newResolution) { onResize(newResolution); });
    window->getInput().setCallback([this](const Vulcant::VulcantInputValue& key) { onKey(key); });
}

Cadaver::~Cadaver() {}

void Cadaver::changeResolution(const glm::ivec2& newResolution)
{
    resolution = newResolution;
    if (viewController)
    {
        viewController->updateAspect(resolution);
    }
    if (renderer)
    {
        renderer->setResolution(resolution);
    }
}

Vulcant::VulcantWindow& Cadaver::getWindow()
{
    return *window;
}

bool Cadaver::isQuitRequested() const
{
    return quitRequested;
}

GatedDatabase& Cadaver::getGatedDatabase()
{
    return gatedDatabase;
}

Defer& Cadaver::getDefer()
{
    return defer;
}

void Cadaver::onLogic(double delta)
{
    defer.process();

    if (viewController)
    {
        viewController->tick(delta);
    }

    Polyglot::Node countRes          = gatedDatabase.execute({
      {    "type", "GetAssetCount" },
      { "version",               1 }
    });
    size_t         currentAssetCount = countRes.toJson().value("count", static_cast<size_t>(0));
    if (lastAssetCount != currentAssetCount)
    {
        lastAssetCount = currentAssetCount;
        if (viewController)
        {
            viewController->frameOnProject(gatedDatabase);
        }
    }

    renderImGui();
}

void Cadaver::onRender()
{
    if (!renderer || !window)
    {
        return;
    }

    Vulcant::Toolkit::SceneData sceneData{};
    if (viewController)
    {
        sceneData = viewController->getSceneData();
    }

    renderer->render(sceneData);
}

void Cadaver::onResize(const glm::ivec2& newResolution)
{
    changeResolution(newResolution);
}

void Cadaver::onKey(const Vulcant::VulcantInputValue& key)
{
    if (key == Vulcant::VulcantInputValue::F && window && window->getInput().isPressed(Vulcant::VulcantInputValue::Control))
    {
        if (topMenu)
        {
            topMenu->focusSearchBar();
        }
        return;
    }

    if (ImGui::GetCurrentContext())
    {
        if (ImGui::GetIO().WantCaptureMouse)
        {
            if (key == Vulcant::VulcantInputValue::MouseWheelUp || key == Vulcant::VulcantInputValue::MouseWheelDown)
            {
                return;
            }
        }
        if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput)
        {
            return;
        }
    }

    if (viewController)
    {
        viewController->keyEvent(key);
    }
}

void Cadaver::prepare()
{
    gatedDatabase.setDispatchCallback([this](std::function<void()> task) { defer.defer(std::move(task)); });

    windows.setInput(gatedDatabase, defer);

    renderer = std::make_unique<Renderer>(device, *window, resolution, gatedDatabase);

    projectWindow     = std::make_unique<ProjectWindow>();
    topMenu           = std::make_unique<TopMenu>();
    pickingController = std::make_unique<ViewportPickingController>();

    PreferencesWindow preferencesWindow;
    preferencesWindow.load(gatedDatabase, defer);

    Polyglot::Node    camPrefRes = gatedDatabase.execute({
      {    "type", "GetCameraPreferences" },
      { "version",                      1 }
    });
    CameraPreferences cameraPreferences;
    if (camPrefRes.contains("preferences"))
    {
        cameraPreferences.fromNode(camPrefRes["preferences"]);
    }

    viewController = std::make_unique<ViewController>();
    viewController->setMode(cameraPreferences.getCameraMode(), *window, cameraPreferences, resolution);
    viewController->registerUIElements(topMenu->getUIElementFactory(), gatedDatabase, undoStack, windows);
}

void Cadaver::renderImGui()
{
    if (!renderer)
    {
        return;
    }

    renderer->beginUiFrame();

    if (topMenu)
    {
        topMenu->render(gatedDatabase, undoStack, defer, windows, quitRequested);

        if (viewController)
        {
            Polyglot::Node    camPrefRes = gatedDatabase.execute({
              {    "type", "GetCameraPreferences" },
              { "version",                      1 }
            });
            CameraPreferences cameraPreferences;
            if (camPrefRes.contains("preferences"))
            {
                cameraPreferences.fromNode(camPrefRes["preferences"]);
            }

            if (cameraPreferences.getCameraMode() != viewController->getMode())
            {
                viewController->setMode(cameraPreferences.getCameraMode(), *window, cameraPreferences, resolution);
            }

            if (viewController->getCamera() && viewController->getCamera()->isOrthographic() != cameraPreferences.isOrthographic())
            {
                viewController->getCamera()->setOrthographic(cameraPreferences.isOrthographic());
            }
        }
    }

    if (viewController)
    {
        viewController->renderViewCube(resolution,
                                       gatedDatabase,
                                       [this](bool ortho)
                                       {
                                           Polyglot::Node    camPrefRes = gatedDatabase.execute({
                                             {    "type", "GetCameraPreferences" },
                                             { "version",                      1 }
                                           });
                                           CameraPreferences prefs;
                                           if (camPrefRes.contains("preferences"))
                                           {
                                               prefs.fromNode(camPrefRes["preferences"]);
                                           }
                                           prefs.setOrthographic(ortho);
                                           Polyglot::Node command = {
                                               {        "type", "UpdateCameraPreferences" },
                                               {     "version",                         1 },
                                               { "preferences",            prefs.toJson() }
                                           };
                                           defer.defer([this, command]() { gatedDatabase.execute(command); });
                                           if (viewController && viewController->getCamera())
                                           {
                                               viewController->getCamera()->setOrthographic(ortho);
                                           }
                                       });
    }

    if (projectWindow)
    {
        projectWindow->render(gatedDatabase, windows);
    }

    pickingController->update(gatedDatabase, windows, viewController->getSceneData());

    windows.render();
}
