#include "Concept/SvoFileConcept.h"

SvoFileConcept::SvoFileConcept() {}

SvoFileConcept::~SvoFileConcept() {}

std::string SvoFileConcept::getType() const
{
    return "SVO File Format (.svo)";
}

std::string SvoFileConcept::getDescription() const
{
    return "Concept: SVO (Sparse Voxel Octree)\n"
           "File Extension: .svo\n"
           "Format Type: Hierarchical Volumetric Octree\n\n"
           "Overview:\n"
           "Sparse Voxel Octrees are 8-ary tree data structures designed to represent massive 3D volumetric fields efficiently. "
           "Instead of allocating an N^3 dense uniform grid in memory, empty or homogeneous space is pruned, only storing "
           "subdivisions where matter or surface boundaries exist.\n\n"
           "Key Characteristics:\n"
           "  - Level-of-Detail (LOD): Nodes can be coarsened or refined dynamically based on camera distance, screen-space pixel projection, or memory budgets.\n"
           "  - Fast Spatial Queries: Fast ray traversal, collision detection, and spatial partitioning with O(log N) depth.\n"
           "  - In CADaver: Encapsulated in VoxelAsset and meshed using dynamic LOD Marching Cubes (CADacomb::Render::SVOLODMesh).";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SvoFileConcept/TypeAndDescription")
{
    SvoFileConcept item;
    REQUIRE(!item.getType().empty());
    REQUIRE(!item.getDescription().empty());
}
#endif
