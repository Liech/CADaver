#include "Concept/StlFileConcept.h"

StlFileConcept::StlFileConcept() {}

StlFileConcept::~StlFileConcept() {}

std::string StlFileConcept::getType() const
{
    return "STL";
}

std::string StlFileConcept::getDescription() const
{
    return "STL (Stereolithography) is a standard 3D file format that represents surface geometry "
           "as an unstructured collection of triangular facets. It is widely used in 3D printing, "
           "rapid prototyping, and polygonal visualization.";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("StlFileConcept/TypeAndDescription")
{
    StlFileConcept item;
    REQUIRE(!item.getType().empty());
    REQUIRE(!item.getDescription().empty());
}
#endif
