#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class Asset;
class GatedDatabase;
class Windows;

// Context menu for CAD assets providing STEP save and conversion operations.
class CadAssetContextMenu : public AssetContextMenu
{
  public:
    CadAssetContextMenu();
    virtual ~CadAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const Asset& asset) override;
};
