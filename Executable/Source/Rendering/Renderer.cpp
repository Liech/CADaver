#include "Renderer.h"

#include "Commands/GatedDatabase.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"

Renderer::Renderer(Vulcant::VulcantDevice& device, const glm::ivec2& resolution, GatedDatabase& gatedDatabase)
  : device(device)
  , resolution(resolution)
  , gatedDatabase(gatedDatabase)
{
    setResolution(resolution);
}

Renderer::~Renderer() {}

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
        dirty = true;
    }
}

void Renderer::setSceneData(const Vulcant::Toolkit::SceneData& newSceneData)
{
    sceneData = newSceneData;
}

void Renderer::beginRecord()
{
    if (drawCmd)
    {
        drawCmd->wait();
    }
    else
    {
        drawCmd = device.createGraphicCommand();
    }

    drawCmd->startRecord();
    drawCmd->setClearColor(getClearColor());
}

void Renderer::endRecord()
{
    if (drawCmd)
    {
        drawCmd->endRecord();
    }
    dirty = false;
}

void Renderer::execute()
{
    if (drawCmd)
    {
        drawCmd->runAsync();
        drawCmd->wait();
    }
}

bool Renderer::needsRecord() const
{
    return dirty || !drawCmd;
}

void Renderer::markDirty()
{
    dirty = true;
}

const glm::ivec2& Renderer::getResolution() const
{
    return resolution;
}

const Vulcant::Toolkit::SceneData& Renderer::getSceneData() const
{
    return sceneData;
}

glm::vec4 Renderer::getClearColor() const
{
    return gatedDatabase.get().getPreferences().getUIPreferences().getBackgroundColor();
}

Vulcant::Toolkit::MeshRenderer::WireframeMode Renderer::getWireframeMode() const
{
    return static_cast<Vulcant::Toolkit::MeshRenderer::WireframeMode>(gatedDatabase.get().getPreferences().getRenderPreferences().getWireframeMode());
}

glm::vec3 Renderer::getWireframeColor() const
{
    return gatedDatabase.get().getPreferences().getRenderPreferences().getWireframeColor();
}

Vulcant::VulcantCullMode Renderer::getCullMode() const
{
    return static_cast<Vulcant::VulcantCullMode>(gatedDatabase.get().getPreferences().getRenderPreferences().getCullMode());
}

Vulcant::VulcantGraphicCommand* Renderer::getCommand() const
{
    return drawCmd.get();
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
