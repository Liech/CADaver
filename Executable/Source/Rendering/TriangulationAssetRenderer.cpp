#include "TriangulationAssetRenderer.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Rendering/Renderer.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"

TriangulationAssetRenderer::TriangulationAssetRenderer(std::shared_ptr<TriangulationAsset> assetInput)
  : asset(std::move(assetInput))
{
    rebuildVertices();
}

TriangulationAssetRenderer::~TriangulationAssetRenderer() {}

void TriangulationAssetRenderer::rebuildVertices()
{
    cachedVertices.clear();
    if (!asset || !asset->getTriangulation() || asset->getTriangulation()->indices.empty())
    {
        dirty = false;
        return;
    }

    const auto* triangulation = asset->getTriangulation();
    lastColor                 = asset->getMeshColor();
    cachedVertices.reserve(triangulation->indices.size());

    for (size_t i = 0; i + 2 < triangulation->indices.size(); i += 3)
    {
        size_t idx0 = triangulation->indices[i];
        size_t idx1 = triangulation->indices[i + 1];
        size_t idx2 = triangulation->indices[i + 2];

        if (idx0 >= triangulation->vertices.size() || idx1 >= triangulation->vertices.size() || idx2 >= triangulation->vertices.size())
        {
            continue;
        }

        glm::vec3 p0 = glm::vec3(triangulation->vertices[idx0]);
        glm::vec3 p1 = glm::vec3(triangulation->vertices[idx1]);
        glm::vec3 p2 = glm::vec3(triangulation->vertices[idx2]);

        glm::vec3 normal = glm::cross(p1 - p0, p2 - p0);
        float     len    = glm::length(normal);
        if (len > 1e-6f)
        {
            normal /= len;
        }
        else
        {
            normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        cachedVertices.push_back({ p0, normal, lastColor });
        cachedVertices.push_back({ p1, normal, lastColor });
        cachedVertices.push_back({ p2, normal, lastColor });
    }

    dirty = false;
    asset->markClean();
}

void TriangulationAssetRenderer::render(Renderer& renderer, bool isSelected)
{
    if (!asset)
    {
        return;
    }

    if (!meshRenderer)
    {
        meshRenderer = std::make_unique<Vulcant::Toolkit::MeshRenderer>(renderer.getDevice(), renderer.getResolution());
    }

    meshRenderer->setResolution(renderer.getResolution());
    meshRenderer->setOutputTextures(renderer.getColor(), renderer.getDepth(), renderer.getNormal());
    meshRenderer->setSceneData(renderer.getSceneData());
    meshRenderer->setWireframeMode(renderer.getWireframeMode());
    meshRenderer->setWireframeColor(renderer.getWireframeColor());
    meshRenderer->setCullMode(renderer.getCullMode());

    if (dirty || asset->isDirty() || lastColor != asset->getMeshColor())
    {
        rebuildVertices();
        meshRenderer->setVertices(cachedVertices);
    }

    if (cachedVertices.empty() || !renderer.getCommand())
    {
        return;
    }

    if (isSelected)
    {
        glm::vec3                                 highlightColor(0.95f, 0.72f, 0.32f);
        std::vector<Vulcant::Toolkit::MeshVertex> highlighted = cachedVertices;
        for (auto& v : highlighted)
        {
            v.color = highlightColor;
        }
        meshRenderer->setVertices(highlighted);
        meshRenderer->record(*renderer.getCommand());
        meshRenderer->setVertices(cachedVertices);
    }
    else
    {
        meshRenderer->record(*renderer.getCommand());
    }
}
