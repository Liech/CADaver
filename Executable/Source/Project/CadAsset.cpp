#include "CadAsset.h"

#include "Library/CAD/CADShape.h"
#include "Library/Operation/IO/LoadCADOperation.h"
#include "Library/Operation/TriangulateOperation.h"

CadAsset::CadAsset(const std::string& nameInput)
  : Asset(nameInput, AssetType::Cad)
{
}

CadAsset::CadAsset(const std::string& nameInput, const std::string& filePathInput)
  : Asset(nameInput, AssetType::Cad)
{
    loadFromFile(filePathInput);
}

CadAsset::CadAsset(const std::string& nameInput, std::shared_ptr<Library::CADShape> cadShapeInput)
  : Asset(nameInput, AssetType::Cad)
  , cadShape(std::move(cadShapeInput))
{
    triangulateShape();
}

CadAsset::~CadAsset() {}

bool CadAsset::loadFromFile(const std::string& path)
{
    filePath         = path;
    auto loadedShape = Library::LoadCADOperation::loadFromFile(filePath);
    if (!loadedShape)
    {
        return false;
    }

    cadShape = std::move(loadedShape);
    triangulateShape();
    return true;
}

const std::string& CadAsset::getFilePath() const
{
    return filePath;
}

void CadAsset::setFilePath(const std::string& path)
{
    filePath = path;
    dirty    = true;
}

Library::CADShape* CadAsset::getCadShape() const
{
    return cadShape.get();
}

void CadAsset::setCadShape(std::shared_ptr<Library::CADShape> shape)
{
    cadShape = std::move(shape);
    triangulateShape();
}

CADacomb::Triangulation* CadAsset::getTriangulation() const
{
    return triangulation.get();
}

void CadAsset::setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri)
{
    triangulation = std::move(tri);
    dirty         = true;
}

const glm::vec3& CadAsset::getMeshColor() const
{
    return meshColor;
}

void CadAsset::setMeshColor(const glm::vec3& color)
{
    meshColor = color;
    dirty     = true;
}

bool CadAsset::isDirty() const
{
    return dirty;
}

void CadAsset::markDirty()
{
    dirty = true;
}

void CadAsset::markClean()
{
    dirty = false;
}

void CadAsset::triangulateShape()
{
    if (cadShape)
    {
        triangulation = Library::TriangulateOperation::triangulate(*cadShape, 0.1);
    }
    else
    {
        triangulation.reset();
    }
    dirty = true;
}
