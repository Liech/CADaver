#pragma once

#include <memory>

class Asset;
class AssetContextMenu;

// Factory for creating and rendering asset context menus based on asset type.
class AssetContextMenuFactory
{
  public:
    AssetContextMenuFactory();
    virtual ~AssetContextMenuFactory();

    std::unique_ptr<AssetContextMenu> create(const std::shared_ptr<Asset>& asset);
    void                              render(const std::shared_ptr<Asset>& asset);
};
