#pragma once

#include "AssetContextMenu.h"

class Asset;
class CadAsset;

// Context menu for CAD assets providing STEP save operations.
class CadAssetContextMenu : public AssetContextMenu
{
  public:
    CadAssetContextMenu();
    virtual ~CadAssetContextMenu();

    void render(Asset& asset) override;

  private:
    void save(CadAsset& asset);
};
