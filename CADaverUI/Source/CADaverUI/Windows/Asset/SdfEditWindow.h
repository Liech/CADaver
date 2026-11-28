#pragma once

#include "CADacomb/SDF/Sdf.h"
#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>
#include <vector>

// Live inspector and editor window for modifying an SdfAsset's formula and AABB bounds.
class SdfEditWindow : public Window
{
  public:
    SdfEditWindow();
    virtual ~SdfEditWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void loadFromAsset();
    void applyUpdate();
    void syncBuffersFromSdf();
    void syncSdfFromBuffers();

    AssetComboBox     assetComboBox;
    CADacomb::GUID    loadedAssetGuid;
    CADacomb::Sdf     sdf;
    std::vector<char> helperBuffer;
    std::vector<char> expressionBuffer;
    float             boundsMin[3] = { -1.0f, -1.0f, -1.0f };
    float             boundsMax[3] = { 1.0f, 1.0f, 1.0f };
    std::string       errorMessage;
    bool              hasLoaded = false;
};
