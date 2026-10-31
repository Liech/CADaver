#include "Database/Asset/CadAsset.h"

#include "CADacomb/CAD/Interface/Shape.h"
#include "CADacomb/CAD/Interface/TessellationOptions.h"
#include "CADacomb/CAD/Sm/KernelSm.h"

CadAsset::CadAsset()
  : triangulation(std::make_unique<CADacomb::Triangulation>())
{
}

CadAsset::~CadAsset() {}

std::string CadAsset::getType() const
{
    return "Cad";
}

const CADacomb::CAD::Shape& CadAsset::getCadShape() const
{
    return *cadShape;
}

CADacomb::CAD::Shape& CadAsset::getCadShape()
{
    return *cadShape;
}

void CadAsset::setCadShape(std::shared_ptr<CADacomb::CAD::Shape> shape)
{
    cadShape = std::move(shape);
    triangulateShape();
}

const CADacomb::Triangulation& CadAsset::getTriangulation() const
{
    return *triangulation;
}

CADacomb::Triangulation& CadAsset::getTriangulation()
{
    return *triangulation;
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
    CadAsset asset;
    asset.setName("TestCad");
    CHECK(asset.getType() == "Cad");
    REQUIRE(asset.getSizeInBytes() >= sizeof(CadAsset));
}
#endif
