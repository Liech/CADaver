#pragma once

class Asset;

// Abstract base class for asset context menus rendered in the UI.
class AssetContextMenu
{
  public:
    AssetContextMenu();
    virtual ~AssetContextMenu();

    virtual void render(Asset& asset) = 0;
};
