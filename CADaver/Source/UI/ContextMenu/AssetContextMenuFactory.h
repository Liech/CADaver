#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

class AssetContextMenu;

// Factory for creating asset context menus based on asset type.
class AssetContextMenuFactory
{
  public:
    AssetContextMenuFactory();
    virtual ~AssetContextMenuFactory();

    std::unique_ptr<AssetContextMenu> create(const std::string& assetType) const;
    void                              registerMenu(const std::string& assetType, std::function<std::unique_ptr<AssetContextMenu>()> creator);

  private:
    void registerMenus();

    std::map<std::string, std::function<std::unique_ptr<AssetContextMenu>()>> creators;
};
