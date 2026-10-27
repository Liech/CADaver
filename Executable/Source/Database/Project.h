#pragma once

#include "Database/Asset.h"

#include <cstdint>
#include <memory>
#include <vector>

// Headless project model managing assets and active selection without rendering dependencies.
class Project
{
  public:
    Project();
    virtual ~Project();

    void addAsset(std::shared_ptr<Asset> asset);
    void insertAsset(size_t index, std::shared_ptr<Asset> asset);
    void removeAsset(size_t index);
    void clear();

    size_t                                     getAssetCount() const;
    std::shared_ptr<Asset>                     getAsset(size_t index) const;
    const std::vector<std::shared_ptr<Asset>>& getAssets() const;

    int                    getSelectedIndex() const;
    void                   setSelectedIndex(int index);
    std::shared_ptr<Asset> getSelectedAsset() const;

  private:
    std::vector<std::shared_ptr<Asset>> assets;
    int                                 selectedIndex = -1;
};
