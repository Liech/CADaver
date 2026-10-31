#pragma once

#include "CADacomb/Util/GUID.h"

#include <memory>
#include <string>

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
    void renderCloseModal(GatedDatabase& gatedDatabase);

  private:
    std::string    closeTargetName;
    CADacomb::GUID closeTargetGuid;
    bool           openCloseModal = false;
};
