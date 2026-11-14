#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for exporting a volumetric SVO asset to a file on disk.
class SaveVoxelAssetWindow : public Window
{
  public:
    SaveVoxelAssetWindow();
    virtual ~SaveVoxelAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void browseFile();
    void executeSave();

    AssetComboBox assetComboBox;
    std::string   filePath;
};
