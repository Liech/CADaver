#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Non-modal window for clustering triangulation faces by normal angle threshold.
class ClusterByNormalWindow : public Window
{
  public:
    ClusterByNormalWindow();
    virtual ~ClusterByNormalWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeCluster();

    AssetComboBox assetComboBox;
    float         maxAngleDegrees = 35.0f;
    bool          withoutHoles    = false;
};
