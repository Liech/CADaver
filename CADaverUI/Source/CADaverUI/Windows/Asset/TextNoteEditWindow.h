#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Window for viewing, creating, and editing TextNoteComponent instances attached to project assets.
class TextNoteEditWindow : public Window
{
  public:
    TextNoteEditWindow();
    virtual ~TextNoteEditWindow();

    std::string getType() const override;
    std::string getDescription() const override;
    void        render() override;

    void setTarget(const CADacomb::GUID& targetAssetGuid, const CADacomb::GUID& targetComponentGuid);

  private:
    AssetComboBox  assetComboBox;
    CADacomb::GUID currentComponentGuid;
    std::string    cachedText;
    std::string    cachedName;
    bool           needsSync = true;
};
