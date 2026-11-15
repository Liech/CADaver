#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Non-modal window for converting a Triangulation asset naively into a CAD asset.
class ConvertStlToCadWindow : public Window
{
  public:
    ConvertStlToCadWindow();
    virtual ~ConvertStlToCadWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeConvert();

    AssetComboBox assetComboBox;
    float         stitchTolerance = 0.0001f;
    bool          tryMakeSolid    = true;
};
