#pragma once

#include "AssetContextMenu.h"

class Asset;
class TriangulationAsset;
class StlToSvoConversionModal;

// Context menu for triangulation assets providing STL save and conversion operations.
class TriangulationAssetContextMenu : public AssetContextMenu
{
  public:
    TriangulationAssetContextMenu();
    virtual ~TriangulationAssetContextMenu();

    void render(const Asset& asset) override;

    void setStlToSvoModal(StlToSvoConversionModal* modal);

  private:
    void save(const TriangulationAsset& asset);

    StlToSvoConversionModal* stlToSvoModal = nullptr;
};
