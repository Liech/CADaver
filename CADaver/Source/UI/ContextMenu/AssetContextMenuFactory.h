#pragma once

#include <memory>

class Asset;
class AssetContextMenu;
class GatedDatabase;
class WindowContainer;

// Factory for creating and rendering asset context menus based on asset type.
class AssetContextMenuFactory
{
  public:
    AssetContextMenuFactory();
    virtual ~AssetContextMenuFactory();

    std::unique_ptr<AssetContextMenu> create(const std::shared_ptr<const Asset>& asset);

    void render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const std::shared_ptr<const Asset>& asset);
};
