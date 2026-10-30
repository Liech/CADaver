#pragma once

#include "CADacomb/Util/GUID.h"

class Asset;

// Abstract base class for asset context menus rendered in the UI.
class AssetContextMenu
{
  public:
    AssetContextMenu();
    virtual ~AssetContextMenu();

    virtual void render(const Asset& asset) = 0;

    const CADacomb::GUID& getAssetGuid() const;
    void                  setAssetGuid(const CADacomb::GUID& guid);

  private:
    CADacomb::GUID assetGuid;
};
