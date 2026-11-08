#pragma once

#include "Windows/Window.h"

#include <string>

// Dialog window for loading a volumetric SVO file into the project.
class LoadSvoAssetWindow : public Window
{
  public:
    LoadSvoAssetWindow();
    virtual ~LoadSvoAssetWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

  private:
    void browseFile();
    void executeLoad();

    std::string filePath;
    std::string assetName;
};
