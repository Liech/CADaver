#pragma once

#include "IO/AssetSaver/AssetSaver.h"

// Saves an SdfAsset to a versioned JSON file or byte stream callback.
class SdfAssetSaver : public AssetSaver
{
  public:
    SdfAssetSaver();
    virtual ~SdfAssetSaver();

    virtual bool save(const Asset& asset, const Polyglot::Node& destination) const override;
};
