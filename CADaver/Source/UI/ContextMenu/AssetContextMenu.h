#pragma once

class Asset;
class GatedDatabase;
class Windows;

// Abstract base class for asset context menus rendered in the UI.
class AssetContextMenu
{
  public:
    AssetContextMenu();
    virtual ~AssetContextMenu();

    virtual void render(GatedDatabase& gatedDatabase, Windows& windows, const Asset& asset) = 0;
};
