#pragma once

#include "Polyglot/Node.h"

#include <functional>
#include <map>
#include <memory>
#include <string>

class AssetLoader;

// Factory creating AssetLoader instances by asset type or file extension.
class AssetLoaderFactory
{
  public:
    AssetLoaderFactory();
    virtual ~AssetLoaderFactory();

    std::unique_ptr<AssetLoader> create(const std::string& assetType) const;
    std::string                  resolveAssetType(const std::string& assetType, const Polyglot::Node& source, const std::string& name) const;

  private:
    void        registerLoaders();
    std::string extractExtension(const std::string& pathStr) const;

    std::map<std::string, std::function<std::unique_ptr<AssetLoader>()>> creators;
    std::map<std::string, std::string>                                   extensionToType;
};
