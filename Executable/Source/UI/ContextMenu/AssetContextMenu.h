#pragma once

class Asset;

// Abstract base class for asset context menus rendered in the UI.
class AssetContextMenu
{
  public:
    AssetContextMenu();
    virtual ~AssetContextMenu();

    virtual void render(Asset& asset) = 0;

    int  getAssetIndex() const;
    void setAssetIndex(int index);

  private:
    int assetIndex = -1;
};
