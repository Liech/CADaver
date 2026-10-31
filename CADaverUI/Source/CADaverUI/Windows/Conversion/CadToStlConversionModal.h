#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for converting a CAD asset to an STL triangulation asset.
class CadToStlConversionModal : public Window
{
  public:
    CadToStlConversionModal();
    virtual ~CadToStlConversionModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeConversion();

    AssetComboBox assetComboBox;
    double        precision    = 0.1;
    bool          deleteOrigin = false;
};
