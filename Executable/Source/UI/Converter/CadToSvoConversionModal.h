#pragma once

#include "UI/Component/VoxelResolutionComboBox.h"

#include <string>

// Modal dialog for converting a CAD asset to a volumetric SVO asset.
class CadToSvoConversionModal
{
  public:
    CadToSvoConversionModal();
    virtual ~CadToSvoConversionModal();

    void open(int assetIndex, const std::string& assetName);
    void render();

  private:
    void executeConversion();

    VoxelResolutionComboBox resolutionComboBox;
    std::string             targetAssetName;
    int                     targetAssetIndex = -1;
    double                  precision        = 0.1;
    bool                    deleteOrigin     = false;
    bool                    shouldOpenPopup  = false;
};
