#include "Concept/SubAssetConcept.h"

SubAssetConcept::SubAssetConcept() {}

SubAssetConcept::~SubAssetConcept() {}

std::string SubAssetConcept::getType() const
{
    return "SubAsset";
}

std::string SubAssetConcept::getDescription() const
{
    return "A SubAsset is an element that belongs to a parent Asset. "
           "It represents an internal part, body, face, or sub-component of a larger model, "
           "such as individual bodies in an assembly or specific faces of a solid.";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SubAssetConcept/TypeAndDescription")
{
    SubAssetConcept item;
    REQUIRE(item.getType() == "SubAsset");
    REQUIRE(!item.getDescription().empty());
}
#endif
