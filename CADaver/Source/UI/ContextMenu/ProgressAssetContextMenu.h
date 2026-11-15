#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class GatedDatabase;
class Windows;

// Context menu for progress bar assets providing progress details and cancellation.
class ProgressAssetContextMenu : public AssetContextMenu
{
  public:
    ProgressAssetContextMenu();
    virtual ~ProgressAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid) override;
};
