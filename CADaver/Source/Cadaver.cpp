#include "Cadaver.h"

#include "Camera/ViewController.h"
#include "Commands/Converter/ConvertCadToStlCommand.h"
#include "Commands/Converter/ConvertCadToSvoCommand.h"
#include "Commands/Converter/ConvertStlToSvoCommand.h"
#include "Commands/Converter/ConvertSvoToStlCommand.h"
#include "Commands/Generator/GenerateLandscapeCommand.h"
#include "Commands/IO/LoadStepAssetCommand.h"
#include "Commands/IO/LoadStlAssetCommand.h"
#include "Commands/IO/LoadSvoAssetCommand.h"
#include "Commands/IO/SaveStepAssetCommand.h"
#include "Commands/IO/SaveStlAssetCommand.h"
#include "Commands/IO/SaveVoxelAssetCommand.h"
#include "Commands/Preferences/UpdateCameraPreferencesCommand.h"
#include "Commands/Preferences/UpdateRenderPreferencesCommand.h"
#include "Commands/Preferences/UpdateSVOPreferencesCommand.h"
#include "Commands/Preferences/UpdateUIPreferencesCommand.h"
#include "Commands/Project/ClearProjectCommand.h"
#include "Commands/Project/RemoveAssetCommand.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Execution/Defer.h"
#include "Rendering/Renderer.h"
#include "UI/Menu/TopMenu.h"
#include "UI/Preferences/PreferencesWindow.h"
#include "UI/Project/ProjectWindow.h"
#include "Undo/RedoCommand.h"
#include "Undo/UndoCommand.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantInput.h"
#include "Vulcant/Interface/VulcantInputValue.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Vulcant/Toolkit/SceneData.h"
#include "Vulcant/VulcantV/VulcantVDevice.h"
#include "Vulcant/Wrapper/Window.h"

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

    auto cameraPrefs          = gatedDatabase.get().getPreferences().getCameraPreferences().toJson();
    cameraPrefs["resolution"] = { resolution.x, resolution.y };
    Defer::getInstance().defer(
      [this, cameraPrefs]()
      {
          gatedDatabase.execute({
            {        "type", "UpdateCameraPreferences" },
            {     "version",                         1 },
            { "preferences",               cameraPrefs }
          });
      });
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
    Defer::getInstance().process();

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
    registerCommands();

    renderer = std::make_unique<Renderer>(device, *window, resolution, gatedDatabase);

    const auto& preferences = gatedDatabase.get().getPreferences();

    projectWindow = std::make_unique<ProjectWindow>();
    topMenu       = std::make_unique<TopMenu>();
    if (topMenu->getPreferencesWindow())
    {
        topMenu->getPreferencesWindow()->getUIPreferencesView().apply(preferences.getUIPreferences());
        topMenu->getPreferencesWindow()->load(gatedDatabase);
    }

    viewController = std::make_unique<ViewController>();
    viewController->setMode(preferences.getCameraPreferences().getCameraMode(), *window, preferences.getCameraPreferences(), resolution);
}

void Cadaver::registerCommands()
{
    auto& factory = gatedDatabase.getCommandFactory();
    factory.registerCommand("RemoveAsset", []() { return std::make_unique<RemoveAssetCommand>(); });
    factory.registerCommand("ClearProject", []() { return std::make_unique<ClearProjectCommand>(); });
    factory.registerCommand("ConvertCadToStl", []() { return std::make_unique<ConvertCadToStlCommand>(); });
    factory.registerCommand("ConvertCadToSvo", []() { return std::make_unique<ConvertCadToSvoCommand>(); });
    factory.registerCommand("ConvertStlToSvo", []() { return std::make_unique<ConvertStlToSvoCommand>(); });
    factory.registerCommand("ConvertSvoToStl", []() { return std::make_unique<ConvertSvoToStlCommand>(); });
    factory.registerCommand("GenerateLandscape", []() { return std::make_unique<GenerateLandscapeCommand>(); });
    factory.registerCommand("LoadStlAsset", []() { return std::make_unique<LoadStlAssetCommand>(); });
    factory.registerCommand("SaveStlAsset", []() { return std::make_unique<SaveStlAssetCommand>(); });
    factory.registerCommand("LoadStepAsset", []() { return std::make_unique<LoadStepAssetCommand>(); });
    factory.registerCommand("SaveStepAsset", []() { return std::make_unique<SaveStepAssetCommand>(); });
    factory.registerCommand("LoadSvoAsset", []() { return std::make_unique<LoadSvoAssetCommand>(); });
    factory.registerCommand("SaveVoxelAsset", []() { return std::make_unique<SaveVoxelAssetCommand>(); });
    factory.registerCommand("Undo", []() { return std::make_unique<UndoCommand>(); });
    factory.registerCommand("Redo", []() { return std::make_unique<RedoCommand>(); });
    factory.registerCommand("UpdateCameraPreferences", []() { return std::make_unique<UpdateCameraPreferencesCommand>(); });
    factory.registerCommand("UpdateUIPreferences", []() { return std::make_unique<UpdateUIPreferencesCommand>(); });
    factory.registerCommand("UpdateRenderPreferences", []() { return std::make_unique<UpdateRenderPreferencesCommand>(); });
    factory.registerCommand("UpdateSVOPreferences", []() { return std::make_unique<UpdateSVOPreferencesCommand>(); });
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
        topMenu->render(gatedDatabase, quitRequested);

        auto* prefWin = topMenu->getPreferencesWindow();
        if (prefWin && viewController)
        {
            topMenu->renderWindows(gatedDatabase, viewController->getCamera());

            if (prefWin->isResetRequested())
            {
                viewController->reset(preferences.getCameraPreferences(), project);
                prefWin->clearResetRequested();
            }

            if (preferences.getCameraPreferences().getCameraMode() != viewController->getMode())
            {
                viewController->setMode(preferences.getCameraPreferences().getCameraMode(), *window, preferences.getCameraPreferences(), resolution);
            }
        }
        else
        {
            topMenu->renderWindows(gatedDatabase);
        }
    }

    if (projectWindow)
    {
        projectWindow->render(gatedDatabase);
    }

    if (viewController)
    {
        viewController->renderViewCube(resolution, project);
    }
}
