#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class Asset;
class GatedDatabase;
class Windows;

// Context menu for voxel assets providing SVO save and conversion operations.
class VoxelAssetContextMenu : public AssetContextMenu
{
  public:
    VoxelAssetContextMenu();
    virtual ~VoxelAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const Asset& asset) override;
};
