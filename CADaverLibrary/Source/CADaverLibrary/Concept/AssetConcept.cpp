#include "Concept/AssetConcept.h"

AssetConcept::AssetConcept() {}

AssetConcept::~AssetConcept() {}

std::string AssetConcept::getType() const
{
    return "Asset";
}

std::string AssetConcept::getDescription() const
{
    return "An Asset represents a top-level model or dataset in the project. "
           "Examples include CAD solids (STEP), polygonal surface meshes (STL), "
           "volumetric voxel octrees (SVO), and background tasks (Progress).";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetConcept/TypeAndDescription")
{
    AssetConcept item;
    REQUIRE(item.getType() == "Asset");
    REQUIRE(!item.getDescription().empty());
}
#endif
