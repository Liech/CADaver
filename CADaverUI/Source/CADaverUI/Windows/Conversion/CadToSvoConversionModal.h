#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/VoxelResolutionComboBox.h"

#include <string>

class GatedDatabase;

// Modal dialog for converting a CAD asset to a volumetric SVO asset.
class CadToSvoConversionModal
{
  public:
    CadToSvoConversionModal();
    virtual ~CadToSvoConversionModal();

    void open(const CADacomb::GUID& assetGuid, const std::string& assetName);
    void render(GatedDatabase& gatedDatabase);

  private:
    void executeConversion(GatedDatabase& gatedDatabase);

    VoxelResolutionComboBox resolutionComboBox;
    std::string             targetAssetName;
    CADacomb::GUID          targetAssetGuid;
    double                  precision       = 0.1;
    bool                    deleteOrigin    = false;
    bool                    shouldOpenPopup = false;
};
