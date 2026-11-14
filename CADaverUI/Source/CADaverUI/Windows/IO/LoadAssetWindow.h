#pragma once

#include "Windows/Window.h"

#include <string>

// Dialog window for loading a CAD (STEP), mesh (STL), or voxel (SVO) file into the project.
class LoadAssetWindow : public Window
{
  public:
    LoadAssetWindow();
    virtual ~LoadAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void browseFile();
    void executeLoad();

    std::string filePath;
    std::string assetName;
};
