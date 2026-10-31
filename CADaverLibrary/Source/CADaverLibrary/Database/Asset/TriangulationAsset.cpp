#include "Database/Asset/TriangulationAsset.h"

TriangulationAsset::TriangulationAsset()
  : triangulation(std::make_unique<CADacomb::Triangulation>())
{
}

TriangulationAsset::~TriangulationAsset() {}

std::string TriangulationAsset::getType() const
{
    return "Triangulation";
}

const CADacomb::Triangulation& TriangulationAsset::getTriangulation() const
{
    return *triangulation;
}

CADacomb::Triangulation& TriangulationAsset::getTriangulation()
{
    return *triangulation;
}

void TriangulationAsset::setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri)
{
    triangulation = std::move(tri);
    markModified();
}

const glm::vec3& TriangulationAsset::getMeshColor() const
{
    return meshColor;
}

void TriangulationAsset::setMeshColor(const glm::vec3& color)
{
    meshColor = color;
    markModified();
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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TriangulationAsset/getSizeInBytes")
{
    TriangulationAsset asset;
    asset.setName("TestAsset");
    CHECK(asset.getType() == "Triangulation");
    REQUIRE(asset.getSizeInBytes() >= sizeof(TriangulationAsset));

    auto tri = std::make_unique<CADacomb::Triangulation>();
    tri->vertices.push_back(glm::dvec3(0.0));
    tri->indices.push_back(0);
    asset.setTriangulation(std::move(tri));
    REQUIRE(asset.getSizeInBytes() > sizeof(TriangulationAsset));
}
#endif
