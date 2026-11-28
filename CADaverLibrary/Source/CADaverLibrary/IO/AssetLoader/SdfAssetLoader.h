#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads versioned JSON SDF files or byte streams into an SdfAsset.
class SdfAssetLoader : public AssetLoader
{
  public:
    SdfAssetLoader();
    virtual ~SdfAssetLoader();

    virtual std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
