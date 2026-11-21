#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for converting a volumetric SVO asset to a Marching Cubes triangulation STL asset.
class SvoToStlMarchingCubesConversionModal : public Window
{
  public:
    SvoToStlMarchingCubesConversionModal();
    virtual ~SvoToStlMarchingCubesConversionModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeConversion();
    void updateMaxDepthFromSelectedAsset(bool resetTargetToMax);

    AssetComboBox  assetComboBox;
    CADacomb::GUID lastTrackedGuid;
    int            targetDepth  = 6;
    int            maxTreeDepth = 6;
    bool           deleteOrigin = false;
};
