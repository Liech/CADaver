#pragma once

class Asset;
class GatedDatabase;

// Abstract base class for asset context menus rendered in the UI.
class AssetContextMenu
{
  public:
    AssetContextMenu();
    virtual ~AssetContextMenu();

    virtual void render(GatedDatabase& gatedDatabase, const Asset& asset) = 0;
};
