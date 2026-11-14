#define CADAVER_ALLOW_GATED_GET

#include "Renderer.h"

#include "AssetRenderFactory.h"
#include "AssetRenderer.h"
#include "Database/Asset.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Interface/VulcantUi.h"
#include "Vulcant/Interface/VulcantWindow.h"

#include <imgui.h>

#include <ImGuizmo.h>
#include <set>

Renderer::Renderer(Vulcant::VulcantDevice& device, Vulcant::VulcantWindow& window, const glm::ivec2& resolution, GatedDatabase& gatedDatabase)
  : device(device)
  , window(window)
  , resolution(resolution)
  , gatedDatabase(gatedDatabase)
{
    ui = device.createUi(window, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT, false);
    setResolution(resolution);
}

Renderer::~Renderer()
{
    renderers.clear();

    if (uiCmdG)
    {
        uiCmdG->wait();
    }
    if (ui)
    {
        ui.reset();
    }
    if (drawCmd)
    {
        drawCmd->wait();
    }
}

void Renderer::setResolution(const glm::ivec2& newResolution)
{
    if (resolution != newResolution || !colorImage)
    {
        resolution = newResolution;
        if (resolution.x > 0 && resolution.y > 0)
        {
            colorImage  = device.createImage(resolution.x, resolution.y, 1, Vulcant::VulcantImageFormat::R32G32B32A32_SFLOAT);
            normalImage = device.createImage(resolution.x, resolution.y, 1, Vulcant::VulcantImageFormat::R16G16B16A16_SFLOAT);
            depthImage  = device.createImage(resolution.x, resolution.y, 1, Vulcant::VulcantImageFormat::D32_SFLOAT);
        }
        clearRenderer.reset();
        if (drawCmd)
        {
            drawCmd->wait();
            drawCmd.reset();
        }
        recordUiCommand();
    }
}

void Renderer::render(const Vulcant::Toolkit::SceneData& sceneData)
{
    clear();

    const auto& project = gatedDatabase.get().getProject();
    pruneRenderers(project.getAssets());

    for (const auto& asset : project.getAssets())
    {
        if (asset)
        {
            renderAsset(asset, sceneData);
        }
    }

    renderUi();
}

void Renderer::clear()
{
    recordClearPass();
}

void Renderer::renderAsset(const std::shared_ptr<const Asset>& asset, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (!asset)
    {
        return;
    }

    auto it = renderers.find(asset->getGuid());
    if (it == renderers.end())
    {
        auto newRenderer = AssetRenderFactory::getInstance().create(asset->getType());
        if (newRenderer)
        {
            newRenderer->setAsset(asset);
            it = renderers.emplace(asset->getGuid(), std::move(newRenderer)).first;
        }
    }

    if (it != renderers.end() && it->second)
    {
        it->second->render(*this, gatedDatabase, sceneData);
    }
}

void Renderer::pruneRenderers(const std::vector<std::shared_ptr<const Asset>>& activeAssets)
{
    std::set<CADacomb::GUID> activeGuids;
    for (const auto& a : activeAssets)
    {
        if (a)
        {
            activeGuids.insert(a->getGuid());
        }
    }

    for (auto it = renderers.begin(); it != renderers.end();)
    {
        if (activeGuids.find(it->first) == activeGuids.end())
        {
            it = renderers.erase(it);
        }
        else
        {
            it++;
        }
    }
}

void Renderer::beginUiFrame()
{
    if (ui)
    {
        ui->newFrame();
        ImGuizmo::BeginFrame();
        ImGuizmo::SetRect(0.0f, 0.0f, static_cast<float>(resolution.x), static_cast<float>(resolution.y));
    }
}

void Renderer::renderUi()
{
    if (!ui || !uiCmdG || !colorImage)
    {
        return;
    }

    Vulcant::VulcantImage& resultImage = *colorImage;

    uiCmdG->wait();
    uiCmdG->startRecord();
    ui->record(*uiCmdG, resultImage);
    uiCmdG->endRecord();
    uiCmdG->runAsync();
    uiCmdG->wait();

    window.blitImage(resultImage);
}

glm::vec4 Renderer::getClearColor() const
{
    Polyglot::Node response = gatedDatabase.execute({
      {    "type", "GetUIPreferences" },
      { "version",                  1 }
    });
    UIPreferences uiPreferences;
    if (response.contains("preferences"))
    {
        uiPreferences.fromNode(response["preferences"]);
    }
    return uiPreferences.getBackgroundColor();
}

Vulcant::VulcantDevice& Renderer::getDevice() const
{
    return device;
}

Vulcant::VulcantImage& Renderer::getColor() const
{
    return *colorImage;
}

Vulcant::VulcantImage& Renderer::getDepth() const
{
    return *depthImage;
}

Vulcant::VulcantImage& Renderer::getNormal() const
{
    return *normalImage;
}

Vulcant::VulcantImage& Renderer::getResult() const
{
    return *colorImage;
}

void Renderer::recordClearPass()
{
    if (!colorImage || !depthImage || !normalImage)
    {
        return;
    }

    if (!drawCmd)
    {
        drawCmd = device.createGraphicCommand();
    }
    else
    {
        drawCmd->wait();
    }

    if (!clearRenderer)
    {
        clearRenderer = std::make_unique<Vulcant::Toolkit::MeshRenderer>(device, resolution);
        clearRenderer->setOutputTextures(*colorImage, *depthImage, *normalImage);

        std::vector<Vulcant::Toolkit::MeshVertex> dummyVertices = {
            { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
            { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) },
            { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) }
        };
        clearRenderer->setVertices(dummyVertices);
    }

    drawCmd->startRecord();
    drawCmd->setClearColor(getClearColor());
    clearRenderer->record(*drawCmd);
    drawCmd->endRecord();
    drawCmd->runAsync();
    drawCmd->wait();
}

void Renderer::recordUiCommand()
{
    if (uiCmdG)
    {
        uiCmdG->wait();
    }
    uiCmdG = device.createGraphicCommand();
}
