#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Component/VoxelResolutionComboBox.h"
#include "Windows/Window.h"

#include <string>

// Modal dialog for converting a CAD asset to a volumetric SVO asset.
class CadToSvoConversionModal : public Window
{
  public:
    CadToSvoConversionModal();
    virtual ~CadToSvoConversionModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;
    virtual bool        isOpen() const override;
    virtual void        open() override;
    virtual void        close() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeConversion();

    AssetComboBox           assetComboBox;
    VoxelResolutionComboBox resolutionComboBox;
    double                  precision       = 0.1;
    bool                    deleteOrigin    = false;
    bool                    openState       = false;
    bool                    shouldOpenPopup = false;
};
