#pragma once

#include "AssetContextMenu.h"

class Asset;
class TriangulationAsset;

// Context menu for triangulation assets providing STL save operations.
class TriangulationAssetContextMenu : public AssetContextMenu
{
  public:
    TriangulationAssetContextMenu();
    virtual ~TriangulationAssetContextMenu();

    void render(Asset& asset) override;

  private:
    void save(TriangulationAsset& asset);
};
