#include "Concept/StepFileConcept.h"

StepFileConcept::StepFileConcept() {}

StepFileConcept::~StepFileConcept() {}

std::string StepFileConcept::getType() const
{
    return "STEP";
}

std::string StepFileConcept::getDescription() const
{
    return "STEP (Standard for the Exchange of Product model data) is an ISO standard exchange format "
           "for Computer-Aided Design (CAD). It represents solid and surface geometry using exact mathematical "
           "boundary representations (B-Rep), preserving curves and planes without polygon tessellation.";
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
