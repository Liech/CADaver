#include "Concept/SvoFileConcept.h"

SvoFileConcept::SvoFileConcept() {}

SvoFileConcept::~SvoFileConcept() {}

std::string SvoFileConcept::getType() const
{
    return "SVO";
}

std::string SvoFileConcept::getDescription() const
{
    return "SVO (Sparse Voxel Octree) is a hierarchical volumetric data structure that represents "
           "3D space as subdivided cubic cells (voxels). Only occupied regions are stored in tree nodes, "
           "enabling efficient rendering and multi-resolution level-of-detail (LOD).";
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
