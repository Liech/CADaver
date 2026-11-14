#pragma once

#include "CADacomb/Util/GUID.h"

#include <memory>
#include <string>

class AssetContextMenuFactory;
class GatedDatabase;
class Windows;

// Renders asset context menus inline or as named popups using AssetContextMenuFactory.
class AssetContextMenuRenderer
{
  public:
    AssetContextMenuRenderer();
    virtual ~AssetContextMenuRenderer();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType);
    void renderPopup(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType, const std::string& popupId);

  private:
    void renderMenuContents(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType);

    std::unique_ptr<AssetContextMenuFactory> factory;
};
