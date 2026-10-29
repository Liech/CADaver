#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/AssetType.h"

#include <string>

// Headless base class for project data assets. Must remain decoupled from rendering and graphics APIs.
class Asset
{
  public:
    Asset(const std::string& name, AssetType type);
    Asset(const CADacomb::GUID& guid, const std::string& name, AssetType type);
    virtual ~Asset();

    const CADacomb::GUID& getGuid() const;

    const std::string& getName() const;
    void               setName(const std::string& name);

    AssetType   getType() const;
    std::string getTypeName() const;

    virtual size_t getSizeInBytes() const = 0;

  private:
    CADacomb::GUID guid;
    std::string    name;
    AssetType      type;
};
