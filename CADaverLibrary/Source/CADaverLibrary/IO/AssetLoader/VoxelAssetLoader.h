#pragma once

#include "IO/AssetLoader/AssetLoader.h"

// Loads SVO voxel files or byte streams into a VoxelAsset.
class VoxelAssetLoader : public AssetLoader
{
  public:
    VoxelAssetLoader();
    virtual ~VoxelAssetLoader();

    virtual std::shared_ptr<Asset> load(const std::string& name, const Polyglot::Node& source) const override;
};
