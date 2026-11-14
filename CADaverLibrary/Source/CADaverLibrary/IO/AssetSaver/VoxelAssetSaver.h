#pragma once

#include "IO/AssetSaver/AssetSaver.h"

// Saves a VoxelAsset to an SVO file or byte stream callback.
class VoxelAssetSaver : public AssetSaver
{
  public:
    VoxelAssetSaver();
    virtual ~VoxelAssetSaver();

    virtual bool save(const Asset& asset, const Polyglot::Node& destination) const override;
};
