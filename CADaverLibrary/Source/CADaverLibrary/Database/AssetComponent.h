#pragma once

#include "CADacomb/Util/GUID.h"

#include <cstddef>
#include <string>

// Headless base class for data components attached to an Asset.
class AssetComponent
{
  public:
    AssetComponent();
    virtual ~AssetComponent();

    const CADacomb::GUID& getGuid() const;

    const std::string& getName() const;
    void               setName(const std::string& name);

    virtual std::string getType() const                                  = 0;
    virtual std::string getDescription() const                           = 0;
    virtual size_t      getSizeInBytes() const                           = 0;
    virtual bool        isCompatible(const std::string& assetType) const = 0;

  private:
    CADacomb::GUID guid;
    std::string    name;
};
