#pragma once

#include "Windows/Window.h"

#include <string>

// Dialog window for loading an STL triangulation file into the project.
class LoadStlAssetWindow : public Window
{
  public:
    LoadStlAssetWindow();
    virtual ~LoadStlAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void browseFile();
    void executeLoad();

    std::string filePath;
    std::string assetName;
};
