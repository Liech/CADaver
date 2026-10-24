#include "TriangulationAsset.h"

TriangulationAsset::TriangulationAsset(const std::string& nameInput)
  : Asset(nameInput, AssetType::Triangulation)
{
}

TriangulationAsset::TriangulationAsset(const std::string& nameInput, std::unique_ptr<CADacomb::Triangulation> tri)
  : Asset(nameInput, AssetType::Triangulation)
  , triangulation(std::move(tri))
{
}

TriangulationAsset::~TriangulationAsset() {}

CADacomb::Triangulation* TriangulationAsset::getTriangulation() const
{
    return triangulation.get();
}

void TriangulationAsset::setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri)
{
    triangulation = std::move(tri);
    dirty         = true;
}

const glm::vec3& TriangulationAsset::getMeshColor() const
{
    return meshColor;
}

void TriangulationAsset::setMeshColor(const glm::vec3& color)
{
    meshColor = color;
    dirty     = true;
}

bool TriangulationAsset::isDirty() const
{
    return dirty;
}

void TriangulationAsset::markClean()
{
    dirty = false;
}
