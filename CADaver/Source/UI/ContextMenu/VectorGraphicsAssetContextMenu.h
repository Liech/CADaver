#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

// Context menu actions for VectorGraphicsAsset entries in the Project window.
class VectorGraphicsAssetContextMenu : public AssetContextMenu
{
  public:
    VectorGraphicsAssetContextMenu();
    virtual ~VectorGraphicsAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid) override;
};
