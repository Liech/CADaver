#include "Database/Asset/CadAsset.h"

#include "CADaverLibrary/CAD/CADShape.h"
#include "CADaverLibrary/Operation/IO/LoadCADOperation.h"
#include "CADaverLibrary/Operation/TriangulateOperation.h"

CadAsset::CadAsset(const std::string& nameInput)
  : Asset(nameInput, "Cad")
{
}

CadAsset::CadAsset(const std::string& nameInput, const std::string& filePathInput)
  : Asset(nameInput, "Cad")
{
    loadFromFile(filePathInput);
}

CadAsset::CadAsset(const std::string& nameInput, std::shared_ptr<Library::CADShape> cadShapeInput)
  : Asset(nameInput, "Cad")
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
    markModified();
}

const Library::CADShape* CadAsset::getCadShape() const
{
    return cadShape.get();
}

Library::CADShape* CadAsset::getCadShape()
{
    return cadShape.get();
}

void CadAsset::setCadShape(std::shared_ptr<Library::CADShape> shape)
{
    cadShape = std::move(shape);
    triangulateShape();
}

const CADacomb::Triangulation* CadAsset::getTriangulation() const
{
    return triangulation.get();
}

CADacomb::Triangulation* CadAsset::getTriangulation()
{
    return triangulation.get();
}

void CadAsset::setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri)
{
    triangulation = std::move(tri);
    markModified();
}

const glm::vec3& CadAsset::getMeshColor() const
{
    return meshColor;
}

void CadAsset::setMeshColor(const glm::vec3& color)
{
    meshColor = color;
    markModified();
}

size_t CadAsset::getSizeInBytes() const
{
    size_t size = sizeof(CadAsset);
    size += getName().capacity();
    size += filePath.capacity();

    if (triangulation)
    {
        size += sizeof(CADacomb::Triangulation);
        size += triangulation->vertices.capacity() * sizeof(glm::dvec3);
        size += triangulation->indices.capacity() * sizeof(size_t);
    }

    if (cadShape)
    {
        std::vector<const Library::CADShape*> stack;
        stack.push_back(cadShape.get());
        while (!stack.empty())
        {
            const Library::CADShape* current = stack.back();
            stack.pop_back();
            if (current)
            {
                size += sizeof(Library::CADShape);
                const auto& children = current->getChildren();
                size += children.capacity() * sizeof(std::shared_ptr<Library::CADShape>);
                for (const auto& child : children)
                {
                    if (child)
                    {
                        stack.push_back(child.get());
                    }
                }
            }
        }
    }

    return size;
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
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CadAsset/getSizeInBytes")
{
    CadAsset asset("TestCad");
    REQUIRE(asset.getSizeInBytes() >= sizeof(CadAsset));
}
#endif
