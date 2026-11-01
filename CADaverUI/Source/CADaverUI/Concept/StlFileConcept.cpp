#include "Concept/StlFileConcept.h"

StlFileConcept::StlFileConcept() {}

StlFileConcept::~StlFileConcept() {}

std::string StlFileConcept::getType() const
{
    return "STL";
}

std::string StlFileConcept::getDescription() const
{
    return "Concept: STL (Stereolithography)\n"
           "File Extension: .stl\n"
           "Format Variants: Binary and ASCII\n\n"
           "Overview:\n"
           "STL is one of the most widely used 3D formats in rapid prototyping, 3D printing, and computer-aided manufacturing. "
           "It represents 3D surfaces as an unstructured collection of planar triangles defined by three 3D vertices and a face normal vector.\n\n"
           "Key Characteristics:\n"
           "  - Faceted Geometry: Approximates smooth curved surfaces via discrete triangular facets. Higher detail requires higher triangle counts.\n"
           "  - No Topological Connectivity: In raw STL files, adjacent triangles do not share indexed vertices, though CADaver constructs an indexed mesh upon import.\n"
           "  - In CADaver: Managed via TriangulationAsset and rendered using standard hardware vertex/index buffers or converted to SVO voxels.";
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
