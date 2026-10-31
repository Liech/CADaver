#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class Asset;
class GatedDatabase;
class WindowContainer;

// Context menu for voxel assets providing SVO save and conversion operations.
class VoxelAssetContextMenu : public AssetContextMenu
{
  public:
    VoxelAssetContextMenu();
    virtual ~VoxelAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset) override;
};
