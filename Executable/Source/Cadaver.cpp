#include "Cadaver.h"

#include "FileMenu.h"
#include "PreferencesWindow.h"
#include "Project/ProjectWindow.h"
#include "Project/TriangulationAsset.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantInput.h"
#include "Vulcant/Interface/VulcantInputValue.h"
#include "Vulcant/Interface/VulcantUi.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/Toolkit/ArcCam.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
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
    if (drawCmd)
    {
        drawCmd->wait();
    }
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
    if (meshRenderer)
    {
        meshRenderer->setResolution(resolution);
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
    if (camera)
    {
        camera->tick(delta);
    }

    if (project.getAssetCount() != lastAssetCount)
    {
        lastAssetCount = project.getAssetCount();
        frameCameraOnProject();
    }

    renderImGui();
}

void Cadaver::onRender()
{
    if (!meshRenderer || !ui || !uiCmdG || !window)
    {
        return;
    }

    Vulcant::Toolkit::SceneData sceneData{};
    if (camera)
    {
        sceneData = camera->getScene();
    }

    std::vector<Vulcant::Toolkit::MeshVertex> sceneVertices;
    int                                       selectedIndex = project.getSelectedIndex();
    for (size_t i = 0; i < project.getAssetCount(); i++)
    {
        auto asset = project.getAsset(i);
        if (asset)
        {
            bool isSelected = (static_cast<int>(i) == selectedIndex);
            asset->render(sceneVertices, isSelected);
        }
    }

    if (sceneVertices.empty())
    {
        sceneVertices.push_back({ glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) });
        sceneVertices.push_back({ glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) });
        sceneVertices.push_back({ glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) });
    }

    meshRenderer->setVertices(sceneVertices);
    meshRenderer->setSceneData(sceneData);

    if (drawCmd)
    {
        drawCmd->wait();
    }
    drawCmd = device.createGraphicCommand();
    drawCmd->startRecord();
    drawCmd->setClearColor(backgroundColor);
    meshRenderer->record(*drawCmd);
    drawCmd->endRecord();
    drawCmd->runAsync();
    drawCmd->wait();

    Vulcant::VulcantImage& resultImage = meshRenderer->getColor();

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

void Cadaver::prepare()
{
    ui = device.createUi(*window, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT, false);

    preferencesWindow = std::make_unique<PreferencesWindow>();
    preferencesData.load();
    preferencesData.apply();

    projectWindow = std::make_unique<ProjectWindow>();
    fileMenu      = std::make_unique<FileMenu>();

    meshRenderer = std::make_unique<Vulcant::Toolkit::MeshRenderer>(device, resolution);

    camera = std::make_unique<Vulcant::Toolkit::ArcCam>(*window, glm::dvec3(0.0, 3.0, 5.0), glm::dvec3(0.0, 0.0, 0.0));
    camera->setAspect(static_cast<float>(resolution.x) / static_cast<float>(resolution.y));
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

    if (ImGui::BeginMainMenuBar())
    {
        if (fileMenu)
        {
            fileMenu->render(project, *preferencesWindow, quitRequested);
        }

        if (ImGui::BeginMenu("Edit"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About CADaver"))
            {
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
        preferencesWindow->render(preferencesData);
    }
}
