#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

class AssetSaver;

// Factory creating AssetSaver instances by asset type.
class AssetSaverFactory
{
  public:
    AssetSaverFactory();
    virtual ~AssetSaverFactory();

    std::unique_ptr<AssetSaver> create(const std::string& assetType) const;

  private:
    void registerSavers();

    std::map<std::string, std::function<std::unique_ptr<AssetSaver>()>> creators;
};
