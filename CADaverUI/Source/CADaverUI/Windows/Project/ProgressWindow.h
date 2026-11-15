#pragma once

#include "CADacomb/Util/GUID.h"
#include "Windows/Window.h"

#include <string>

// Non-modal window displaying all active progress bar tiers for a ProgressBarAsset.
class ProgressWindow : public Window
{
  public:
    ProgressWindow();
    virtual ~ProgressWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    CADacomb::GUID targetAssetGuid;
};
