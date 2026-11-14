#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class GatedDatabase;
class Windows;

// Context menu for triangulation assets providing STL save and conversion operations.
class TriangulationAssetContextMenu : public AssetContextMenu
{
  public:
    TriangulationAssetContextMenu();
    virtual ~TriangulationAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid) override;
};
