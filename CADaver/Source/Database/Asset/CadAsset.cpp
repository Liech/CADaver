#include "Database/Asset/CadAsset.h"

#include "CADacomb/CAD/Interface/Shape.h"
#include "CADacomb/CAD/Interface/TessellationOptions.h"
#include "CADacomb/CAD/Sm/KernelSm.h"

CadAsset::CadAsset(const std::string& nameInput)
  : Asset(nameInput, "Cad")
{
}

CadAsset::CadAsset(const std::string& nameInput, const std::string& filePathInput)
  : Asset(nameInput, "Cad")
{
    loadFromFile(filePathInput);
}

CadAsset::CadAsset(const std::string& nameInput, std::shared_ptr<CADacomb::CAD::Shape> cadShapeInput)
  : Asset(nameInput, "Cad")
  , cadShape(std::move(cadShapeInput))
{
    triangulateShape();
}

CadAsset::~CadAsset() {}

bool CadAsset::loadFromFile(const std::string& path)
{
    filePath = path;

    CADacomb::CAD::KernelSm kernel;
    auto                    shapes = kernel.loadStepFile(filePath);
    if (shapes.empty() || !shapes[0])
    {
        return false;
    }

    cadShape = std::move(shapes[0]);
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

const CADacomb::CAD::Shape* CadAsset::getCadShape() const
{
    return cadShape.get();
}

CADacomb::CAD::Shape* CadAsset::getCadShape()
{
    return cadShape.get();
}

void CadAsset::setCadShape(std::shared_ptr<CADacomb::CAD::Shape> shape)
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
        size += sizeof(CADacomb::CAD::Shape);
    }

    return size;
}

void CadAsset::triangulateShape()
{
    if (cadShape)
    {
        CADacomb::CAD::KernelSm            kernel;
        CADacomb::CAD::TessellationOptions options;
        options.setChordHeightTolerance(0.1);
        triangulation = kernel.tessellate(*cadShape, options);
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
