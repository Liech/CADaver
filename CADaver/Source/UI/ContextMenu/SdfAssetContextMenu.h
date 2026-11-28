#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class GatedDatabase;
class Windows;

// Context menu for SDF assets providing live editing, saving, and SVO/STL conversion operations.
class SdfAssetContextMenu : public AssetContextMenu
{
  public:
    SdfAssetContextMenu();
    virtual ~SdfAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid) override;
};
