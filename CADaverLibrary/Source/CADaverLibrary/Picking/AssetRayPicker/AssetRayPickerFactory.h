#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

class AssetRayPicker;

// Factory for creating type-specific AssetRayPicker instances.
class AssetRayPickerFactory
{
  public:
    AssetRayPickerFactory();
    virtual ~AssetRayPickerFactory();

    std::unique_ptr<AssetRayPicker> create(const std::string& assetType) const;
    void                            registerPicker(const std::string& assetType, std::function<std::unique_ptr<AssetRayPicker>()> creator);

  private:
    void registerPickers();

    std::map<std::string, std::function<std::unique_ptr<AssetRayPicker>()>> creators;
};
