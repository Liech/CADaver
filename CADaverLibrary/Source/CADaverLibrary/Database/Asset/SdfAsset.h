#pragma once

#include "CADacomb/Formula/Formula.h"
#include "CADacomb/SDF/Sdf.h"
#include "CADacomb/Util/AABB3.h"
#include "Database/Asset.h"

#include <string>

// Headless data asset wrapping CADacomb::Sdf.
class SdfAsset : public Asset
{
  public:
    SdfAsset();
    virtual ~SdfAsset();

    virtual std::string getType() const override;
    virtual size_t      getSizeInBytes() const override;

    const CADacomb::Sdf& getSdf() const;
    void                 setSdf(const CADacomb::Sdf& sdf);

    const CADacomb::Formula& getFormula() const;
    void                     setFormula(const CADacomb::Formula& formula);

    const CADacomb::AABB3& getAABB() const;
    void                   setAABB(const CADacomb::AABB3& aabb);

  private:
    CADacomb::Sdf sdf;
};
