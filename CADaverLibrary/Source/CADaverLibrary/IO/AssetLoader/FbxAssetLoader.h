#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads an FBX source into a TriangulationAsset using CADacomb::IO::FbxReader.
class FbxAssetLoader : public AssetLoader
{
  public:
    FbxAssetLoader();
    virtual ~FbxAssetLoader();

    std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
