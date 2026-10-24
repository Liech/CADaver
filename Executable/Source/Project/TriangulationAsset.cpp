#include "TriangulationAsset.h"

#include "Vulcant/Interface/VulcantDevice.h"
#include "Vulcant/Interface/VulcantGraphicCommand.h"
#include "Vulcant/Toolkit/MeshRenderer.h"
#include "Vulcant/Toolkit/SceneData.h"

TriangulationAsset::TriangulationAsset(const std::string& nameInput)
  : Asset(nameInput, AssetType::Triangulation)
{
}

TriangulationAsset::TriangulationAsset(const std::string& nameInput, std::unique_ptr<CADacomb::Triangulation> tri)
  : Asset(nameInput, AssetType::Triangulation)
  , triangulation(std::move(tri))
{
}

TriangulationAsset::~TriangulationAsset()
{
    if (drawCmd)
    {
        drawCmd->wait();
    }
}

void TriangulationAsset::render(std::vector<Vulcant::Toolkit::MeshVertex>& vertices, bool isSelected)
{
    if (!triangulation || triangulation->indices.empty())
    {
        return;
    }

    glm::vec3 activeColor = isSelected ? glm::vec3(0.95f, 0.72f, 0.32f) : meshColor;
    vertices.reserve(vertices.size() + triangulation->indices.size());

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

        vertices.push_back({ p0, normal, activeColor });
        vertices.push_back({ p1, normal, activeColor });
        vertices.push_back({ p2, normal, activeColor });
    }
}

Vulcant::VulcantImage* TriangulationAsset::render(Vulcant::VulcantDevice& device, const glm::ivec2& resolution, const Vulcant::Toolkit::SceneData& sceneData)
{
    if (resolution.x <= 0 || resolution.y <= 0)
    {
        return nullptr;
    }

    if (!meshRenderer || currentResolution != resolution)
    {
        currentResolution = resolution;
        meshRenderer      = std::make_unique<Vulcant::Toolkit::MeshRenderer>(device, currentResolution);
        meshDirty         = true;
    }

    if (meshDirty)
    {
        if (triangulation && !triangulation->indices.empty())
        {
            meshRenderer->setTriangulation(*triangulation, meshColor);
        }
        else
        {
            meshRenderer->setVertices({});
        }
        meshDirty = false;
    }

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

    return &meshRenderer->getColor();
}

CADacomb::Triangulation* TriangulationAsset::getTriangulation() const
{
    return triangulation.get();
}

void TriangulationAsset::setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri)
{
    triangulation = std::move(tri);
    meshDirty     = true;
}

const glm::vec3& TriangulationAsset::getMeshColor() const
{
    return meshColor;
}

void TriangulationAsset::setMeshColor(const glm::vec3& color)
{
    meshColor = color;
    meshDirty = true;
}
