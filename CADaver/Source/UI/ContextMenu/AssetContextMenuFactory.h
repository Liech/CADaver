#pragma once

#include "CADacomb/Util/GUID.h"

#include <memory>
#include <string>

class AssetContextMenu;
class GatedDatabase;
class Windows;

// Factory for creating and rendering asset context menus based on asset type.
class AssetContextMenuFactory
{
  public:
    AssetContextMenuFactory();
    virtual ~AssetContextMenuFactory();

    std::unique_ptr<AssetContextMenu> create(const std::string& assetType);

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType);
};
