#pragma once

#include "UI/Component/VoxelResolutionComboBox.h"

#include <string>

// Modal dialog for converting a triangulation STL asset to a volumetric SVO asset.
class StlToSvoConversionModal
{
  public:
    StlToSvoConversionModal();
    virtual ~StlToSvoConversionModal();

    void open(int assetIndex, const std::string& assetName);
    void render();

  private:
    void executeConversion();

    VoxelResolutionComboBox resolutionComboBox;
    std::string             targetAssetName;
    int                     targetAssetIndex = -1;
    bool                    deleteOrigin     = false;
    bool                    shouldOpenPopup  = false;
};
