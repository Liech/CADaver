#include "Cadaver.h"

#include "PreferencesWindow.h"
#include "Vulcant/Interface/VulcantComputeCommand.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantResourceLayout.h"
#include "Vulcant/Interface/VulcantUi.h"
#include "Vulcant/Interface/VulcantWindow.h"
#include "Vulcant/VulcantV/VulcantVDevice.h"
#include "Vulcant/Wrapper/Window.h"

#include <imgui.h>

void Cadaver::run()
{
    auto                              windowExtensions = Vulcant::Wrapper::Window::getVulkanExtensions();
    Vulcant::VulcantV::VulcantVDevice device(windowExtensions);
    {
        Cadaver app;
        app.createWindow(device, glm::ivec2(1280, 720));
        auto& window = app.getWindow();

        while (!window.isClosed() && !app.isQuitRequested())
        {
            window.tick();
        }
    }
}

Cadaver::Cadaver() {}

Cadaver::~Cadaver()
{
    if (computeCmd)
    {
        computeCmd->wait();
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

void Cadaver::createWindow(Vulcant::VulcantDevice& deviceInput, const glm::ivec2& res)
{
    resolution = res;
    window     = deviceInput.createWindow(resolution, "CADaver");

    prepare(deviceInput, resolution);
    recordCommand();

    window->start([this](double delta) { onLogic(delta); }, [this]() { onRender(); }, [this](const glm::ivec2& newResolution) { onResize(newResolution); });
}

void Cadaver::changeResolution(const glm::ivec2& newResolution)
{
    resolution  = newResolution;
    renderImage = device->createImage(resolution.x, resolution.y, 1, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT);
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
    renderImGui();
}

void Cadaver::onRender()
{
    if (!renderImage || !ui || !uiCmdG || !computeCmd || !window)
    {
        return;
    }

    computeCmd->wait();
    uiCmdG->wait();

    computeCmd->startRecord();
    computeCmd->addBarrier(*renderImage, Vulcant::VulcantResourceLayout::General);
    computeCmd->addBarrier(*renderImage, Vulcant::VulcantResourceLayout::TransferSrc);
    computeCmd->endRecord();
    computeCmd->runAsync();
    computeCmd->wait();

    uiCmdG->startRecord();
    ui->record(*uiCmdG, *renderImage);
    uiCmdG->endRecord();
    uiCmdG->runAsync();

    window->blitImage(*renderImage);
}

void Cadaver::onResize(const glm::ivec2& newResolution)
{
    changeResolution(newResolution);
    recordCommand();
}

void Cadaver::prepare(Vulcant::VulcantDevice& deviceInput, const glm::ivec2& res)
{
    device     = &deviceInput;
    resolution = res;

    computeCmd = device->createComputeCommand();
    ui         = device->createUi(*window, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT, false);

    preferencesWindow = std::make_unique<PreferencesWindow>();
    preferencesData.load();
    preferencesData.apply();

    changeResolution(resolution);
}

void Cadaver::recordCommand()
{
    if (uiCmdG)
    {
        uiCmdG->wait();
    }
    uiCmdG = device->createGraphicCommand();
}

void Cadaver::renderImGui()
{
    ui->newFrame();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Preferences..."))
            {
                preferencesWindow->setOpen(true);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4"))
            {
                quitRequested = true;
            }
            ImGui::EndMenu();
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

    if (preferencesWindow)
    {
        preferencesWindow->render(preferencesData);
    }
}
