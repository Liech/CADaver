#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/VoxelResolutionComboBox.h"

#include <string>

class GatedDatabase;

// Modal dialog for converting a triangulation STL asset to a volumetric SVO asset.
class StlToSvoConversionModal
{
  public:
    StlToSvoConversionModal();
    virtual ~StlToSvoConversionModal();

    void open(const CADacomb::GUID& assetGuid, const std::string& assetName);
    void render(GatedDatabase& gatedDatabase);

  private:
    void executeConversion(GatedDatabase& gatedDatabase);

    VoxelResolutionComboBox resolutionComboBox;
    std::string             targetAssetName;
    CADacomb::GUID          targetAssetGuid;
    bool                    deleteOrigin    = false;
    bool                    shouldOpenPopup = false;
};
