#include "Rendering/Renderer.h"

#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Interface/VulcantImage.h"
#include "Vulcant/Toolkit/MeshRenderer.h"

Renderer::Renderer(Vulcant::VulcantDevice& deviceInput, const glm::ivec2& initialResolution)
  : device(deviceInput)
  , resolution(initialResolution)
{
    meshRenderer = std::make_unique<Vulcant::Toolkit::MeshRenderer>(device, resolution);
}

Renderer::~Renderer()
{
    if (drawCmd)
    {
        drawCmd->wait();
    }
}

void Renderer::setResolution(const glm::ivec2& newResolution)
{
    if (resolution != newResolution)
    {
        resolution = newResolution;
        if (meshRenderer)
        {
            meshRenderer->setResolution(resolution);
        }
        dirty = true;
    }
}

void Renderer::setSceneData(const Vulcant::Toolkit::SceneData& sceneDataInput)
{
    sceneData = sceneDataInput;
    if (meshRenderer)
    {
        meshRenderer->setSceneData(sceneData);
    }
}

void Renderer::setClearColor(const glm::vec4& color)
{
    if (clearColor != color)
    {
        clearColor = color;
        dirty      = true;
    }
}

void Renderer::setWireframeMode(Vulcant::Toolkit::MeshRenderer::WireframeMode mode)
{
    if (wireframeMode != mode)
    {
        wireframeMode = mode;
        if (meshRenderer)
        {
            meshRenderer->setWireframeMode(wireframeMode);
        }
        dirty = true;
    }
}

void Renderer::setWireframeColor(const glm::vec3& color)
{
    if (wireframeColor != color)
    {
        wireframeColor = color;
        if (meshRenderer)
        {
            meshRenderer->setWireframeColor(wireframeColor);
        }
        dirty = true;
    }
}

void Renderer::setCullMode(Vulcant::VulcantCullMode mode)
{
    if (cullMode != mode)
    {
        cullMode = mode;
        if (meshRenderer)
        {
            meshRenderer->setCullMode(cullMode);
        }
        dirty = true;
    }
}

void Renderer::beginRecord()
{
    accumulatedVertices.clear();
    customBuffers.clear();
}

void Renderer::drawVertices(const std::vector<Vulcant::Toolkit::MeshVertex>& vertices)
{
    accumulatedVertices.insert(accumulatedVertices.end(), vertices.begin(), vertices.end());
}

void Renderer::drawVertices(const std::vector<Vulcant::Toolkit::MeshVertex>& vertices, const glm::vec3& colorOverride)
{
    accumulatedVertices.reserve(accumulatedVertices.size() + vertices.size());
    for (const auto& vertex : vertices)
    {
        accumulatedVertices.push_back({ vertex.position, vertex.normal, colorOverride });
    }
}

void Renderer::drawVertexBuffer(Vulcant::VulcantBuffer& buffer, uint32_t vertexCount)
{
    customBuffers.push_back({ &buffer, vertexCount });
}

void Renderer::endRecord()
{
    if (!meshRenderer)
    {
        return;
    }

    if (drawCmd)
    {
        drawCmd->wait();
    }
    else
    {
        drawCmd = device.createGraphicCommand();
    }

    drawCmd->startRecord();
    drawCmd->setClearColor(clearColor);

    if (!accumulatedVertices.empty())
    {
        meshRenderer->setVertices(accumulatedVertices);
        meshRenderer->record(*drawCmd);
    }
    else if (customBuffers.empty())
    {
        accumulatedVertices.push_back({ glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) });
        accumulatedVertices.push_back({ glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) });
        accumulatedVertices.push_back({ glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f) });
        meshRenderer->setVertices(accumulatedVertices);
        meshRenderer->record(*drawCmd);
    }

    for (const auto& custom : customBuffers)
    {
        if (custom.first && custom.second > 0)
        {
            meshRenderer->record(*drawCmd, *custom.first, custom.second);
        }
    }

    drawCmd->endRecord();

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

Vulcant::VulcantGraphicCommand* Renderer::getCommand() const
{
    return drawCmd.get();
}

const Vulcant::Toolkit::SceneData& Renderer::getSceneData() const
{
    return sceneData;
}

Vulcant::VulcantDevice& Renderer::getDevice() const
{
    return device;
}

Vulcant::VulcantImage& Renderer::getResult() const
{
    return meshRenderer->getColor();
}
