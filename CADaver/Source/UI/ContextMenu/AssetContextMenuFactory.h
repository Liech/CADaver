#pragma once

#include <memory>

class Asset;
class AssetContextMenu;
class GatedDatabase;
class Windows;

// Factory for creating and rendering asset context menus based on asset type.
class AssetContextMenuFactory
{
  public:
    AssetContextMenuFactory();
    virtual ~AssetContextMenuFactory();

    std::unique_ptr<AssetContextMenu> create(const std::shared_ptr<const Asset>& asset);

    void render(GatedDatabase& gatedDatabase, Windows& windows, const std::shared_ptr<const Asset>& asset);
};
