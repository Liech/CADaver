#pragma once

#include "AssetContextMenu.h"

class Asset;
class CadAsset;
class CadToStlConversionModal;
class CadToSvoConversionModal;

// Context menu for CAD assets providing STEP save and conversion operations.
class CadAssetContextMenu : public AssetContextMenu
{
  public:
    CadAssetContextMenu();
    virtual ~CadAssetContextMenu();

    void render(const Asset& asset) override;

    void setCadToStlModal(CadToStlConversionModal* modal);
    void setCadToSvoModal(CadToSvoConversionModal* modal);

  private:
    void save(const CadAsset& asset);

    CadToStlConversionModal* cadToStlModal = nullptr;
    CadToSvoConversionModal* cadToSvoModal = nullptr;
};
