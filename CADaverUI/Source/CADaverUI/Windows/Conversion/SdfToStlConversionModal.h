#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Component/VoxelResolutionComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for converting an SdfAsset to a TriangulationAsset using SurfaceNets.
class SdfToStlConversionModal : public Window
{
  public:
    SdfToStlConversionModal();
    virtual ~SdfToStlConversionModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeConversion();

    AssetComboBox           assetComboBox;
    VoxelResolutionComboBox resolutionComboBox;
    bool                    deleteOrigin = false;
};
