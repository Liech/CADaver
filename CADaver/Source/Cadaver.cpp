#include "Cadaver.h"

#include "Camera/ViewController.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/Preferences.h"
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

void Cadaver::onLogic(double delta)
{
    defer.process();

    if (viewController)
    {
        viewController->tick(delta);
    }

    const auto& project = gatedDatabase.get().getProject();
    if (lastAssetCount != project.getAssetCount())
    {
        lastAssetCount = project.getAssetCount();
        if (viewController)
        {
            viewController->frameOnProject(project);
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
    if (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse)
    {
        if (key == Vulcant::VulcantInputValue::MouseWheelUp || key == Vulcant::VulcantInputValue::MouseWheelDown)
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

    const auto& preferences = gatedDatabase.get().getPreferences();

    projectWindow = std::make_unique<ProjectWindow>();
    topMenu       = std::make_unique<TopMenu>();

    PreferencesWindow preferencesWindow;
    preferencesWindow.load(gatedDatabase, defer);

    viewController = std::make_unique<ViewController>();
    viewController->setMode(preferences.getCameraPreferences().getCameraMode(), *window, preferences.getCameraPreferences(), resolution);
}

void Cadaver::renderImGui()
{
    if (!renderer)
    {
        return;
    }

    renderer->beginUiFrame();

    const auto& database    = gatedDatabase.get();
    const auto& project     = database.getProject();
    const auto& preferences = database.getPreferences();

    if (topMenu)
    {
        topMenu->render(gatedDatabase, undoStack, defer, windows, quitRequested);

        if (viewController)
        {
            if (preferences.getCameraPreferences().getCameraMode() != viewController->getMode())
            {
                viewController->setMode(preferences.getCameraPreferences().getCameraMode(), *window, preferences.getCameraPreferences(), resolution);
            }

            if (viewController->getCamera() && viewController->getCamera()->isOrthographic() != preferences.getCameraPreferences().isOrthographic())
            {
                viewController->getCamera()->setOrthographic(preferences.getCameraPreferences().isOrthographic());
            }
        }
    }

    if (projectWindow)
    {
        projectWindow->render(gatedDatabase, windows);
    }

    windows.render();

    if (viewController)
    {
        viewController->renderViewCube(resolution, project);
    }
}
