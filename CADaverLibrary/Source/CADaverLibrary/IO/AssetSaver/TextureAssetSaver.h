#pragma once

#include "IO/AssetSaver/AssetSaver.h"

// Saves a TextureAsset to a PNG destination.
class TextureAssetSaver : public AssetSaver
{
  public:
    TextureAssetSaver();
    virtual ~TextureAssetSaver();

    bool save(const Asset& asset, const Polyglot::Node& destination) const override;
};
