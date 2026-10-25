#pragma once

#include "AssetContextMenu.h"

class Asset;
class VoxelAsset;

class VoxelAssetContextMenu : public AssetContextMenu
{
  public:
    VoxelAssetContextMenu();
    virtual ~VoxelAssetContextMenu();

    void render(Asset& asset) override;

  private:
    void save(VoxelAsset& asset);
};
