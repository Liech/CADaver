#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Window for configuring a SoftBodyComponent attached to a TriangulationAsset.
class SoftBodyEditWindow : public Window
{
  public:
    SoftBodyEditWindow();
    virtual ~SoftBodyEditWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTarget(const CADacomb::GUID& targetAssetGuid, const CADacomb::GUID& targetComponentGuid);

  private:
    void dispatchUpdate(const CADacomb::GUID& selectedAssetGuid);

    AssetComboBox  assetComboBox;
    CADacomb::GUID currentComponentGuid;
    std::string    cachedName;
    float          cachedMass             = 1.0f;
    float          cachedPressure         = 2000.0f;
    float          cachedEdgeCompliance   = 0.001f;
    float          cachedBendCompliance   = 0.01f;
    float          cachedDamping          = 0.2f;
    float          cachedFriction         = 0.3f;
    float          cachedRestitution      = 0.0f;
    int            cachedSolverIterations = 10;
    bool           needsSync              = true;
};
