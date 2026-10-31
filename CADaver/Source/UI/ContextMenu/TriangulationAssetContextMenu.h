#pragma once

#include "AssetContextMenu.h"

class Asset;
class GatedDatabase;
class StlToSvoConversionModal;
class TriangulationAsset;

// Context menu for triangulation assets providing STL save and conversion operations.
class TriangulationAssetContextMenu : public AssetContextMenu
{
  public:
    TriangulationAssetContextMenu();
    virtual ~TriangulationAssetContextMenu();

    void render(GatedDatabase& gatedDatabase, const Asset& asset) override;

    void setStlToSvoModal(StlToSvoConversionModal* modal);

  private:
    void save(GatedDatabase& gatedDatabase, const TriangulationAsset& asset);

    StlToSvoConversionModal* stlToSvoModal = nullptr;
};
