#pragma once

#include "CADacomb/Util/GUID.h"
#include "Component/AssetComboBox.h"
#include "Windows/Window.h"

#include <string>

// Dialog window for exporting a project asset to disk.
class SaveAssetWindow : public Window
{
  public:
    SaveAssetWindow();
    virtual ~SaveAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void setTargetAsset(const CADacomb::GUID& assetGuid);

  private:
    void        browseFile();
    void        executeSave();
    std::string getDefaultExtension(const std::string& assetType) const;

    AssetComboBox assetComboBox;
    std::string   filePath;
};
