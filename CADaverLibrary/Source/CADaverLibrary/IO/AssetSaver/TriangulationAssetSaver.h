#pragma once

#include "IO/AssetSaver/AssetSaver.h"

// Saves a TriangulationAsset to an STL file or byte stream callback.
class TriangulationAssetSaver : public AssetSaver
{
  public:
    TriangulationAssetSaver();
    virtual ~TriangulationAssetSaver();

    virtual bool save(const Asset& asset, const Polyglot::Node& destination) const override;
};
