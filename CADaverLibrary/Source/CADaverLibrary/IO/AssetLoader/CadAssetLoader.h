#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads STEP CAD files or byte streams into a CadAsset.
class CadAssetLoader : public AssetLoader
{
  public:
    CadAssetLoader();
    virtual ~CadAssetLoader();

    virtual std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
