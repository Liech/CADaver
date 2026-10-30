#include "LoadCADOperation.h"

#include "CADaverLibrary/CAD/CADShape.h"

namespace Library
{
    LoadCADOperation::LoadCADOperation() {}

    LoadCADOperation::~LoadCADOperation() {}

    std::unique_ptr<CADShape> LoadCADOperation::loadFromFile(const std::string& filename)
    {
        return CADShape::load(filename);
    }
}

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Volume/SVO/SVOVoxelizer.h"
#include "CADaverLibrary/Operation/TriangulateOperation.h"
#include "CADaverLibrary/catch.hpp"
#include <algorithm>
#include <filesystem>

TEST_CASE("LoadCADOperation/LoadAndTriangulateStep")
{
    std::filesystem::path stepPath = "Bo.step";
    for (int i = 0; i < 6; i++)
    {
        if (std::filesystem::exists(stepPath))
        {
            break;
        }
        stepPath = ".." / stepPath;
    }
    REQUIRE(std::filesystem::exists(stepPath));

    auto shape = Library::LoadCADOperation::loadFromFile(stepPath.string());
    REQUIRE(shape != nullptr);

    auto tri = Library::TriangulateOperation::triangulate(*shape, 0.1);
    REQUIRE(tri != nullptr);
    REQUIRE(!tri->vertices.empty());
    REQUIRE(!tri->indices.empty());
}

TEST_CASE("LoadCADOperation/VoxelizeZAxisHolderFinal")
{
    std::filesystem::path stepPath = "ZAxisHolderFinal.step";
    for (int i = 0; i < 6; i++)
    {
        if (std::filesystem::exists(stepPath))
        {
            break;
        }
        stepPath = ".." / stepPath;
    }
    REQUIRE(std::filesystem::exists(stepPath));

    auto shape = Library::LoadCADOperation::loadFromFile(stepPath.string());
    REQUIRE(shape != nullptr);

    auto tri = Library::TriangulateOperation::triangulate(*shape, 0.1);
    REQUIRE(tri != nullptr);
    REQUIRE(!tri->vertices.empty());
    REQUIRE(!tri->indices.empty());

    auto       aabb     = tri->getAABB();
    double     maxDim   = std::max({ aabb.second.x, aabb.second.y, aabb.second.z });
    double     margin   = maxDim * 0.02;
    glm::dvec3 start    = aabb.first - glm::dvec3(margin);
    double     cubeSize = maxDim + margin * 2.0;
    glm::dvec3 end      = start + glm::dvec3(cubeSize);

    CADacomb::SVOVoxelizer voxelizer;
    auto                   svo = voxelizer.voxelize<bool>(*tri, start, end, 64);
    REQUIRE(svo != nullptr);

    size_t solidCount = 0;
    for (int z = 0; z < 64; z++)
    {
        for (int y = 0; y < 64; y++)
        {
            for (int x = 0; x < 64; x++)
            {
                if (svo->getVoxel(glm::ivec3(x, y, z)))
                {
                    solidCount++;
                }
            }
        }
    }
    CHECK(solidCount > 0);
}
#endif