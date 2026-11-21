#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Small window for configuring a RigidBodyComponent attached to a TriangulationAsset.
class RigidBodyEditWindow : public Window
{
  public:
    RigidBodyEditWindow();
    virtual ~RigidBodyEditWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTarget(const CADacomb::GUID& targetAssetGuid, const CADacomb::GUID& targetComponentGuid);

  private:
    AssetComboBox  assetComboBox;
    CADacomb::GUID currentComponentGuid;
    std::string    cachedName;
    bool           cachedStatic = false;
    bool           needsSync    = true;
};
