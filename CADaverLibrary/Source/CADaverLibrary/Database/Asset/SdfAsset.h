#pragma once

#include "CADacomb/Formula/Formula.h"
#include "CADacomb/Util/AABB3.h"
#include "Database/Asset.h"

#include <string>

// Headless data asset representing a Signed Distance Field defined by a CADacomb::Formula within an AABB.
class SdfAsset : public Asset
{
  public:
    SdfAsset();
    virtual ~SdfAsset();

    virtual std::string getType() const override;
    virtual size_t      getSizeInBytes() const override;

    const CADacomb::Formula& getFormula() const;
    void                     setFormula(const CADacomb::Formula& formula);

    const CADacomb::AABB3& getAABB() const;
    void                   setAABB(const CADacomb::AABB3& aabb);

  private:
    CADacomb::Formula formula;
    CADacomb::AABB3   aabb;
};
