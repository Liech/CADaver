#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for confirming and removing a target asset from the project.
class RemoveAssetModal : public Window
{
  public:
    RemoveAssetModal();
    virtual ~RemoveAssetModal();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void executeRemove();

    AssetComboBox assetComboBox;
};
