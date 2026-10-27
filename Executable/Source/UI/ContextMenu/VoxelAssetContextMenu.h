#pragma once

#include "AssetContextMenu.h"

class Asset;
class VoxelAsset;
class SvoToStlConversionModal;

// Context menu for voxel assets providing SVO save and conversion operations.
class VoxelAssetContextMenu : public AssetContextMenu
{
  public:
    VoxelAssetContextMenu();
    virtual ~VoxelAssetContextMenu();

    void render(Asset& asset) override;

    void setSvoToStlModal(SvoToStlConversionModal* modal);

  private:
    void save(VoxelAsset& asset);

    SvoToStlConversionModal* svoToStlModal = nullptr;
};
