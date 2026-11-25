#pragma once

#include "IO/AssetSaver/AssetSaver.h"

// Saves VectorGraphicsAsset instances to SVG files or memory destinations.
class VectorGraphicsAssetSaver : public AssetSaver
{
  public:
    VectorGraphicsAssetSaver();
    virtual ~VectorGraphicsAssetSaver();

    bool save(const Asset& asset, const Polyglot::Node& destination) const override;
};
