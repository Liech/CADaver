#pragma once

#include "CADacomb/Util/GUID.h"
#include "Database/Asset.h"

#include <memory>
#include <vector>

// Headless project model managing assets without rendering dependencies.
class Project
{
  public:
    Project();
    virtual ~Project();

    void addAsset(std::shared_ptr<Asset> asset);
    void removeAsset(const CADacomb::GUID& guid);
    void clear();

    size_t                                     getAssetCount() const;
    bool                                       hasAsset(const CADacomb::GUID& guid) const;
    std::shared_ptr<const Asset>               getAsset(const CADacomb::GUID& guid) const;
    std::shared_ptr<Asset>                     getAsset(const CADacomb::GUID& guid);
    std::vector<std::shared_ptr<const Asset>>  getAssets() const;
    const std::vector<std::shared_ptr<Asset>>& getAssets();

  private:
    std::vector<std::shared_ptr<Asset>> assets;
};
