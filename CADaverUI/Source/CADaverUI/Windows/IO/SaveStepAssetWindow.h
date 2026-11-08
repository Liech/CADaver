#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

class Project;

// Dialog window for exporting a CAD asset to a STEP file on disk.
class SaveStepAssetWindow : public Window
{
  public:
    SaveStepAssetWindow();
    virtual ~SaveStepAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void browseFile(const Project& project);
    void executeSave();

    AssetComboBox assetComboBox;
    std::string   filePath;
};
