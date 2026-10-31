#pragma once

class Asset;
class GatedDatabase;
class WindowContainer;

// Abstract base class for asset context menus rendered in the UI.
class AssetContextMenu
{
  public:
    AssetContextMenu();
    virtual ~AssetContextMenu();

    virtual void render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset) = 0;
};
