#pragma once

#include "AssetContextMenu.h"

class Asset;
class GatedDatabase;
class SvoToStlConversionModal;
class VoxelAsset;

// Context menu for voxel assets providing SVO save and conversion operations.
class VoxelAssetContextMenu : public AssetContextMenu
{
  public:
    VoxelAssetContextMenu();
    virtual ~VoxelAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, const Asset& asset) override;

    void setSvoToStlModal(SvoToStlConversionModal* modal);

  private:
    void save(GatedDatabase& gatedDatabase, const VoxelAsset& asset);

    SvoToStlConversionModal* svoToStlModal = nullptr;
};
