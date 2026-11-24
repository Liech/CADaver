#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads a PNG image source into a TextureAsset.
class TextureAssetLoader : public AssetLoader
{
  public:
    TextureAssetLoader();
    virtual ~TextureAssetLoader();

    std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
