#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Polyglot/Node.h"
#include "Windows/Window.h"

#include <future>
#include <string>

// Non-modal window for calculating connected components on a Triangulation asset.
class CalculateConnectedComponentsWindow : public Window
{
  public:
    CalculateConnectedComponentsWindow();
    virtual ~CalculateConnectedComponentsWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeCalculate();

    AssetComboBox               assetComboBox;
    bool                        splitIntoAssets = false;
    std::future<Polyglot::Node> pendingFuture;
    CADacomb::GUID              pendingAssetGuid;
};
