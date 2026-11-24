#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

// Context menu actions for TextureAsset entries in the project window.
class TextureAssetContextMenu : public AssetContextMenu
{
  public:
    TextureAssetContextMenu();
    virtual ~TextureAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid) override;
};
