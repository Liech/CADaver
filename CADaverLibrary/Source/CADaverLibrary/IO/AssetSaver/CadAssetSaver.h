#pragma once

#include "IO/AssetSaver/AssetSaver.h"

// Saves a CadAsset to a STEP file or byte stream callback.
class CadAssetSaver : public AssetSaver
{
  public:
    CadAssetSaver();
    virtual ~CadAssetSaver();

    virtual bool save(const Asset& asset, const Polyglot::Node& destination) const override;
};
