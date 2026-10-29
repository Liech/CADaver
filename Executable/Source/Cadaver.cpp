#include "Cadaver.h"

#include "Camera/CameraController.h"
#include "Commands/Defer.h"

#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Rendering/AssetRenderFactory.h"
#include "Rendering/Renderer.h"
#include "UI/About/AboutWindow.h"
#include "UI/Commands/CommandsWindow.h"
#include "UI/Component/ViewCubeWidget.h"
#include "UI/Generator/LandscapeGeneratorWindow.h"
#include "UI/Menu/CreateMenu.h"
#include "UI/Menu/EditMenu.h"
#include "UI/Menu/FileMenu.h"
#include "UI/Preferences/PreferencesWindow.h"
#include "UI/Project/ProjectWindow.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantInput.h"
#include "Vulcant/Interface/VulcantInputValue.h"
#include "Vulcant/Interface/VulcantUi.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Vulcant/Toolkit/SceneData.h"
#include "Vulcant/VulcantV/VulcantVDevice.h"
#include "Vulcant/Wrapper/Window.h"

#include <imgui.h>

#include <ImGuizmo.h>
#include <algorithm>

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
    recordCommand();

    window->start([this](double delta) { onLogic(delta); }, [this]() { onRender(); }, [this](const glm::ivec2& newResolution) { onResize(newResolution); });
    window->getInput().setCallback([this](const Vulcant::VulcantInputValue& key) { onKey(key); });
}

Cadaver::~Cadaver()
{
    if (uiCmdG)
    {
        uiCmdG->wait();
    }
    if (ui)
    {
        ui.reset();
    }
}

void Cadaver::changeResolution(const glm::ivec2& newResolution)
{
    resolution = newResolution;
    if (cameraController)
    {
        cameraController->updateAspect(resolution);
    }
    if (renderer)
    {
        renderer->setResolution(resolution);
    }
    if (assetRenderFactory)
    {
        assetRenderFactory->clearCache();
    }
    auto cameraPrefs          = GatedDatabase::getInstance().get().getPreferences().getCameraPreferences().toJson();
    cameraPrefs["resolution"] = { resolution.x, resolution.y };
    Defer::getInstance().defer(
      [cameraPrefs]()
      {
          GatedDatabase::getInstance().execute({
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

    if (cameraController)
    {
        cameraController->tick(delta);
    }

    const auto& project = GatedDatabase::getInstance().get().getProject();
    if (project.getAssetCount() != lastAssetCount)
    {
        lastAssetCount = project.getAssetCount();
        if (cameraController)
        {
            cameraController->frameOnProject(project);
        }
    }

    renderImGui();
}

void Cadaver::onRender()
{
    if (!renderer || !ui || !uiCmdG || !window)
    {
        return;
    }

    Vulcant::Toolkit::SceneData sceneData{};
    if (cameraController)
    {
        sceneData = cameraController->getSceneData();
    }

    const auto& preferences = GatedDatabase::getInstance().get().getPreferences();
    const auto& camPrefs    = preferences.getCameraPreferences();

    sceneData.nearPlane  = camPrefs.getNearPlane();
    sceneData.farPlane   = camPrefs.getFarPlane();
    sceneData.resolution = camPrefs.getResolution();

    renderer->setSceneData(sceneData);

    const auto& project = GatedDatabase::getInstance().get().getProject();

    renderer->clear();
    for (const auto& asset : project.getAssets())
    {
        if (asset && assetRenderFactory)
        {
            assetRenderFactory->render(*renderer, asset, false);
        }
    }

    Vulcant::VulcantImage& resultImage = renderer->getResult();

    uiCmdG->wait();
    uiCmdG->startRecord();
    ui->record(*uiCmdG, resultImage);
    uiCmdG->endRecord();
    uiCmdG->runAsync();
    uiCmdG->wait();

    window->blitImage(resultImage);
}

void Cadaver::onResize(const glm::ivec2& newResolution)
{
    changeResolution(newResolution);
    recordCommand();
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

    if (cameraController)
    {
        cameraController->keyEvent(key);
    }
}

void Cadaver::prepare()
{
    ui = device.createUi(*window, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT, false);

    const auto& preferences = GatedDatabase::getInstance().get().getPreferences();

    preferencesWindow = std::make_unique<PreferencesWindow>();
    preferencesWindow->getUIPreferencesView().apply(preferences.getUIPreferences());

    aboutWindow              = std::make_unique<AboutWindow>();
    commandsWindow           = std::make_unique<CommandsWindow>();
    projectWindow            = std::make_unique<ProjectWindow>();
    fileMenu                 = std::make_unique<FileMenu>();
    editMenu                 = std::make_unique<EditMenu>();
    createMenu               = std::make_unique<CreateMenu>();
    landscapeGeneratorWindow = std::make_unique<LandscapeGeneratorWindow>();
    viewCubeWidget           = std::make_unique<ViewCubeWidget>();

    assetRenderFactory = std::make_unique<AssetRenderFactory>();
    renderer           = std::make_unique<Renderer>(device, resolution, GatedDatabase::getInstance());

    cameraController = std::make_unique<CameraController>();
    cameraController->setMode(preferences.getCameraPreferences().getCameraMode(), *window, preferences.getCameraPreferences(), resolution);
}

void Cadaver::recordCommand()
{
    if (uiCmdG)
    {
        uiCmdG->wait();
    }
    uiCmdG = device.createGraphicCommand();
}

void Cadaver::renderImGui()
{
    ui->newFrame();

    ImGuizmo::BeginFrame();
    ImGuizmo::SetRect(0.0f, 0.0f, static_cast<float>(resolution.x), static_cast<float>(resolution.y));

    const auto& database    = GatedDatabase::getInstance().get();
    const auto& project     = database.getProject();
    const auto& preferences = database.getPreferences();

    if (ImGui::BeginMainMenuBar())
    {
        if (fileMenu)
        {
            fileMenu->render(*preferencesWindow, quitRequested);
        }

        if (editMenu)
        {
            editMenu->render();
        }

        if (createMenu && landscapeGeneratorWindow)
        {
            createMenu->render(*landscapeGeneratorWindow);
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About CADaver"))
            {
                if (aboutWindow)
                {
                    aboutWindow->setOpen(true);
                }
            }
            if (ImGui::MenuItem("Commands"))
            {
                if (commandsWindow)
                {
                    commandsWindow->setOpen(true);
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (projectWindow)
    {
        projectWindow->render(project);
    }

    if (preferencesWindow && cameraController)
    {
        preferencesWindow->render(preferences, cameraController->getCamera());

        if (preferencesWindow->isResetRequested())
        {
            cameraController->reset(preferences.getCameraPreferences(), project);
            preferencesWindow->clearResetRequested();
        }

        if (preferences.getCameraPreferences().getCameraMode() != cameraController->getMode())
        {
            cameraController->setMode(preferences.getCameraPreferences().getCameraMode(), *window, preferences.getCameraPreferences(), resolution);
        }
    }

    if (aboutWindow)
    {
        aboutWindow->render();
    }

    if (commandsWindow)
    {
        commandsWindow->render();
    }

    if (landscapeGeneratorWindow)
    {
        landscapeGeneratorWindow->render();
    }

    if (viewCubeWidget && cameraController && cameraController->getCamera())
    {
        viewCubeWidget->render(cameraController->getCamera(),
                               resolution,
                               [this]()
                               {
                                   const auto& project = GatedDatabase::getInstance().get().getProject();
                                   if (cameraController)
                                   {
                                       cameraController->frameOnProject(project);
                                   }
                               });
    }
}
