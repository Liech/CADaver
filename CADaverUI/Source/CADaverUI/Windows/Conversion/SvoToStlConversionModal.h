#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

class Project;

// Modal dialog for converting a volumetric SVO asset to a blocky triangulation STL asset.
class SvoToStlConversionModal : public Window
{
  public:
    SvoToStlConversionModal();
    virtual ~SvoToStlConversionModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;
    virtual bool        isOpen() const override;
    virtual void        open() override;
    virtual void        close() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeConversion();
    void updateMaxDepthFromSelectedAsset(const Project& project, bool resetTargetToMax);

    AssetComboBox assetComboBox;
    int           targetDepth     = 6;
    int           maxTreeDepth    = 6;
    bool          deleteOrigin    = false;
    bool          openState       = false;
    bool          shouldOpenPopup = false;
};
