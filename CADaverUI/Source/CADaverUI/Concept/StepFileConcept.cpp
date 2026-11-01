#include "Concept/StepFileConcept.h"

StepFileConcept::StepFileConcept() {}

StepFileConcept::~StepFileConcept() {}

std::string StepFileConcept::getType() const
{
    return "STEP File Format (.step, .stp)";
}

std::string StepFileConcept::getDescription() const
{
    return "Concept: STEP (Standard for the Exchange of Product model data)\n"
           "File Extensions: .step, .stp\n"
           "Standard: ISO 10303-21 (Clear text encoding of the exchange structure)\n\n"
           "Overview:\n"
           "STEP is the industry-standard exchange format for 3D Computer-Aided Design (CAD). Unlike triangle meshes, "
           "STEP represents objects using exact Boundary Representation (B-Rep) geometry, preserving analytical curves "
           "(NURBS, B-splines, planes, cylinders, tori) and exact topological boundaries (vertices, edges, wires, faces, and solids).\n\n"
           "Key Characteristics:\n"
           "  - Exact Mathematical Representation: Does not degrade into discrete planar facets unless explicitly meshed/tessellated.\n"
           "  - Topology Preservation: Maintains adjacency information between faces, edges, and volumes.\n"
           "  - In CADaver: Loaded as CadAsset using the OpenCASCADE technology kernel, allowing dynamic tessellation or voxelization.";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("StepFileConcept/TypeAndDescription")
{
    StepFileConcept item;
    REQUIRE(!item.getType().empty());
    REQUIRE(!item.getDescription().empty());
}
#endif
