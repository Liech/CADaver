#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class Asset;
class GatedDatabase;
class WindowContainer;

// Context menu for CAD assets providing STEP save and conversion operations.
class CadAssetContextMenu : public AssetContextMenu
{
  public:
    CadAssetContextMenu();
    virtual ~CadAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset) override;
};
