#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads STL mesh files or byte streams into a TriangulationAsset.
class TriangulationAssetLoader : public AssetLoader
{
  public:
    TriangulationAssetLoader();
    virtual ~TriangulationAssetLoader();

    virtual std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
