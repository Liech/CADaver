#pragma once

#include "Windows/Window.h"

#include <string>

// Dialog window for loading a STEP/STP CAD file into the project.
class LoadStepAssetWindow : public Window
{
  public:
    LoadStepAssetWindow();
    virtual ~LoadStepAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void browseFile();
    void executeLoad();

    std::string filePath;
    std::string assetName;
};
