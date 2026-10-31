#pragma once

#include "UI/ContextMenu/AssetContextMenu.h"

class Asset;
class GatedDatabase;
class WindowContainer;

// Context menu for triangulation assets providing STL save and conversion operations.
class TriangulationAssetContextMenu : public AssetContextMenu
{
  public:
    TriangulationAssetContextMenu();
    virtual ~TriangulationAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset) override;
};
