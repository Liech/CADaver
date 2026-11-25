#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads SVG vector graphics assets from file paths, strings, or binary memory buffers.
class VectorGraphicsAssetLoader : public AssetLoader
{
  public:
    VectorGraphicsAssetLoader();
    virtual ~VectorGraphicsAssetLoader();

    std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
