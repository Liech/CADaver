#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for simplifying a triangulation mesh asset by percentage or target triangle count.
class SimplifyMeshWindow : public Window
{
  public:
    SimplifyMeshWindow();
    virtual ~SimplifyMeshWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeSimplification();

    AssetComboBox assetComboBox;
    int           modeIndex           = 0;
    float         targetPercent       = 50.0f;
    int           targetTriangleCount = 1000;
    bool          deleteOrigin        = false;
};
