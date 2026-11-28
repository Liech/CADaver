#include "Database/Asset/SdfAsset.h"

SdfAsset::SdfAsset()
  : aabb(glm::dvec3(-1.0), glm::dvec3(1.0))
{
    formula.setName("sdfFormula");
    formula.setInputCount(3);
    formula.setOutputCount(1);
    formula.setExpression("float3 p = float3(input[0], input[1], input[2]);\noutput[0] = length(p) - 0.5;");
}

SdfAsset::~SdfAsset() {}

std::string SdfAsset::getType() const
{
    return "Sdf";
}

size_t SdfAsset::getSizeInBytes() const
{
    return sizeof(SdfAsset) + formula.getName().size() + formula.getExpression().size() + formula.getHelper().size();
}

const CADacomb::Formula& SdfAsset::getFormula() const
{
    return formula;
}

void SdfAsset::setFormula(const CADacomb::Formula& newFormula)
{
    formula = newFormula;
    markModified();
}

const CADacomb::AABB3& SdfAsset::getAABB() const
{
    return aabb;
}

void SdfAsset::setAABB(const CADacomb::AABB3& newAabb)
{
    aabb = newAabb;
    markModified();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SdfAsset/DefaultStateAndModificationRevision")
{
    SdfAsset asset;
    CHECK(asset.getType() == "Sdf");
    CHECK(asset.getSizeInBytes() >= sizeof(SdfAsset));
    CHECK(asset.getFormula().getInputCount() == 3);
    CHECK(asset.getFormula().getOutputCount() == 1);

    size_t            rev0 = asset.getRevision();
    CADacomb::Formula f;
    f.setName("boxSdf");
    f.setInputCount(3);
    f.setOutputCount(1);
    f.setExpression("output[0] = length(float3(input[0], input[1], input[2])) - 0.25;");
    asset.setFormula(f);
    CHECK(asset.getRevision() > rev0);

    size_t rev1 = asset.getRevision();
    asset.setAABB(CADacomb::AABB3(glm::dvec3(-2.0), glm::dvec3(2.0)));
    CHECK(asset.getRevision() > rev1);
}
#endif
