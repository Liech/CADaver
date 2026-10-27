#include "Database/Asset/TriangulationAsset.h"

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

size_t TriangulationAsset::getSizeInBytes() const
{
    size_t size = sizeof(TriangulationAsset);
    size += getName().capacity();

    if (triangulation)
    {
        size += sizeof(CADacomb::Triangulation);
        size += triangulation->vertices.capacity() * sizeof(glm::dvec3);
        size += triangulation->indices.capacity() * sizeof(size_t);
    }

    return size;
}

bool TriangulationAsset::isDirty() const
{
    return dirty;
}

void TriangulationAsset::markClean()
{
    dirty = false;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TriangulationAsset/getSizeInBytes")
{
    TriangulationAsset asset("TestAsset");
    REQUIRE(asset.getSizeInBytes() >= sizeof(TriangulationAsset));

    auto tri = std::make_unique<CADacomb::Triangulation>();
    tri->vertices.push_back(glm::dvec3(0.0));
    tri->indices.push_back(0);
    asset.setTriangulation(std::move(tri));
    REQUIRE(asset.getSizeInBytes() > sizeof(TriangulationAsset));
}
#endif
