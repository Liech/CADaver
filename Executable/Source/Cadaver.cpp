#include "Cadaver.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Rendering/AssetRenderFactory.h"
#include "Rendering/Renderer.h"
#include "UI/About/AboutWindow.h"
#include "UI/Commands/CommandsWindow.h"
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
#include "Vulcant/Toolkit/ArcCam.h"
#include "Vulcant/Toolkit/Freecam.h"
#include "Vulcant/Toolkit/SceneData.h"
#include "Vulcant/VulcantV/VulcantVDevice.h"
#include "Vulcant/Wrapper/Window.h"

#include <algorithm>
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
    if (camera)
    {
        camera->setAspect(static_cast<float>(resolution.x) / static_cast<float>(resolution.y));
    }
    if (renderer)
    {
        renderer->setResolution(resolution);
    }
    GatedDatabase::getInstance().execute({
      {        "type",                            "UpdateCameraPreferences" },
      {     "version",                                                    1 },
      { "preferences", { { "resolution", { resolution.x, resolution.y } } } }
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
    if (camera)
    {
        camera->tick(delta);
    }

    const auto& project = GatedDatabase::getInstance().get().getProject();
    if (project.getAssetCount() != lastAssetCount)
    {
        lastAssetCount = project.getAssetCount();
        if (renderer)
        {
            renderer->markDirty();
        }
        frameCameraOnProject();
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
    if (camera)
    {
        sceneData = camera->getScene();
    }

    const auto& preferences = GatedDatabase::getInstance().get().getPreferences();
    const auto& camPrefs    = preferences.getCameraPreferences();
    const auto& uiPrefs     = preferences.getUIPreferences();
    const auto& renderPrefs = preferences.getRenderPreferences();

    sceneData.nearPlane  = camPrefs.getNearPlane();
    sceneData.farPlane   = camPrefs.getFarPlane();
    sceneData.resolution = camPrefs.getResolution();

    renderer->setClearColor(uiPrefs.getBackgroundColor());
    renderer->setWireframeMode(static_cast<Vulcant::Toolkit::MeshRenderer::WireframeMode>(renderPrefs.getWireframeMode()));
    renderer->setWireframeColor(renderPrefs.getWireframeColor());
    renderer->setCullMode(static_cast<Vulcant::VulcantCullMode>(renderPrefs.getCullMode()));
    renderer->setSceneData(sceneData);

    const auto& project       = GatedDatabase::getInstance().get().getProject();
    int         selectedIndex = project.getSelectedIndex();
    if (selectedIndex != lastSelectedIndex)
    {
        lastSelectedIndex = selectedIndex;
        renderer->markDirty();
    }

    if (renderer->needsRecord())
    {
        renderer->beginRecord();
        for (size_t i = 0; i < project.getAssetCount(); i++)
        {
            auto asset = project.getAsset(i);
            if (asset && assetRenderFactory)
            {
                bool isSelected = (static_cast<int>(i) == selectedIndex);
                assetRenderFactory->render(*renderer, asset, isSelected);
            }
        }
        renderer->endRecord();
    }

    renderer->execute();

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
    if (camera)
    {
        camera->keyEvent(key);
    }
}

void Cadaver::frameCameraOnProject()
{
    glm::dvec3 minBound(1e9);
    glm::dvec3 maxBound(-1e9);
    bool       hasBounds = false;

    const auto& project = GatedDatabase::getInstance().get().getProject();
    for (size_t i = 0; i < project.getAssetCount(); i++)
    {
        auto asset = project.getAsset(i);
        if (asset && asset->getType() == AssetType::Triangulation)
        {
            auto triAsset = std::dynamic_pointer_cast<TriangulationAsset>(asset);
            if (triAsset && triAsset->getTriangulation() && !triAsset->getTriangulation()->vertices.empty())
            {
                auto [pos, size] = triAsset->getTriangulation()->getAABB();
                minBound         = glm::min(minBound, pos);
                maxBound         = glm::max(maxBound, pos + size);
                hasBounds        = true;
            }
        }
        else if (asset && asset->getType() == AssetType::Cad)
        {
            auto cadAsset = std::dynamic_pointer_cast<CadAsset>(asset);
            if (cadAsset && cadAsset->getTriangulation() && !cadAsset->getTriangulation()->vertices.empty())
            {
                auto [pos, size] = cadAsset->getTriangulation()->getAABB();
                minBound         = glm::min(minBound, pos);
                maxBound         = glm::max(maxBound, pos + size);
                hasBounds        = true;
            }
        }
    }

    if (hasBounds && camera)
    {
        glm::dvec3 center = 0.5 * (minBound + maxBound);
        glm::dvec3 size   = maxBound - minBound;
        double     maxDim = std::max({ size.x, size.y, size.z });
        double     dist   = (maxDim > 1e-4) ? maxDim * 1.8 : 5.0;

        glm::dvec3 offset = glm::normalize(glm::dvec3(0.0, 3.0, 5.0)) * dist;
        camera->setTarget(center);
        camera->setEye(center + offset);
        camera->setZNear(static_cast<float>(std::max(0.01, dist * 0.005)));
        camera->setZFar(static_cast<float>(std::max(2000.0, dist * 50.0)));
    }
}

void Cadaver::setCameraMode(int mode)
{
    if (!window)
    {
        return;
    }

    const auto& camPrefs = GatedDatabase::getInstance().get().getPreferences().getCameraPreferences();
    glm::dvec3  eye      = camera ? camera->getEye() : glm::dvec3(0.0, 3.0, 5.0);
    glm::dvec3  target   = camera ? camera->getTarget() : glm::dvec3(0.0, 0.0, 0.0);
    glm::dvec3  up       = camera ? camera->getUp() : glm::dvec3(0.0, 1.0, 0.0);
    float       zNear    = camPrefs.getNearPlane();
    float       zFar     = camPrefs.getFarPlane();
    float       fov      = camPrefs.getFov();
    float       aspect   = camera ? camera->getAspect() : ((resolution.y > 0) ? static_cast<float>(resolution.x) / static_cast<float>(resolution.y) : 16.0f / 9.0f);
    double      speed    = static_cast<double>(camPrefs.getCameraSpeed());
    double      rotSpeed = camPrefs.getRotationSensitivity();

    if (mode == 1)
    {
        camera = std::make_unique<Vulcant::Toolkit::Freecam>(*window, eye, target, up);
    }
    else
    {
        camera = std::make_unique<Vulcant::Toolkit::ArcCam>(*window, eye, target, up);
    }

    camera->setZNear(zNear);
    camera->setZFar(zFar);
    camera->setFov(fov);
    camera->setAspect(aspect);
    camera->setTranslationSensitivity(speed);
    camera->setRotationSensitivity(rotSpeed);
}

void Cadaver::resetCamera()
{
    if (GatedDatabase::getInstance().get().getProject().getAssetCount() > 0)
    {
        frameCameraOnProject();
    }
    else if (camera)
    {
        const auto& camPrefs = GatedDatabase::getInstance().get().getPreferences().getCameraPreferences();
        camera->setTarget(glm::dvec3(0.0, 0.0, 0.0));
        camera->setEye(glm::dvec3(0.0, 3.0, 5.0));
        camera->setUp(glm::dvec3(0.0, 1.0, 0.0));
        camera->setZNear(0.05f);
        camera->setZFar(2000.0f);
        camera->setFov(camPrefs.getFov());
        camera->setRotationSensitivity(camPrefs.getRotationSensitivity());
        camera->setAspect((resolution.y > 0) ? static_cast<float>(resolution.x) / static_cast<float>(resolution.y) : 16.0f / 9.0f);
    }
}

void Cadaver::prepare()
{
    ui = device.createUi(*window, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT, false);

    const auto& preferences = GatedDatabase::getInstance().get().getPreferences();

    preferencesWindow = std::make_unique<PreferencesWindow>();
    preferencesWindow->getUIPreferencesView().apply(preferences.getUIPreferences());

    aboutWindow    = std::make_unique<AboutWindow>();
    commandsWindow = std::make_unique<CommandsWindow>();
    projectWindow  = std::make_unique<ProjectWindow>();
    fileMenu       = std::make_unique<FileMenu>();

    assetRenderFactory = std::make_unique<AssetRenderFactory>();
    renderer           = std::make_unique<Renderer>(device, resolution);
    renderer->setClearColor(preferences.getUIPreferences().getBackgroundColor());
    renderer->setWireframeMode(static_cast<Vulcant::Toolkit::MeshRenderer::WireframeMode>(preferences.getRenderPreferences().getWireframeMode()));
    renderer->setWireframeColor(preferences.getRenderPreferences().getWireframeColor());
    renderer->setCullMode(static_cast<Vulcant::VulcantCullMode>(preferences.getRenderPreferences().getCullMode()));

    setCameraMode(preferences.getCameraPreferences().getCameraMode());
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

    const auto& database    = GatedDatabase::getInstance().get();
    const auto& project     = database.getProject();
    const auto& preferences = database.getPreferences();

    if (ImGui::BeginMainMenuBar())
    {
        if (fileMenu)
        {
            fileMenu->render(*preferencesWindow, quitRequested);
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

    if (preferencesWindow)
    {
        preferencesWindow->render(preferences, camera.get());

        if (preferences.getCameraPreferences().isResetRequested())
        {
            resetCamera();
            GatedDatabase::getInstance().execute({
              {        "type",           "UpdateCameraPreferences" },
              {     "version",                                   1 },
              { "preferences", { { "clearResetRequested", true } } }
            });
        }

        if (preferences.getCameraPreferences().hasModeChangeRequest())
        {
            setCameraMode(preferences.getCameraPreferences().getRequestedMode());
            GatedDatabase::getInstance().execute({
              {        "type",              "UpdateCameraPreferences" },
              {     "version",                                      1 },
              { "preferences", { { "clearModeChangeRequest", true } } }
            });
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
}
