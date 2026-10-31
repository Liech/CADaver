#pragma once

#include "CADacomb/Volume/SVO/Generator/LandscapeConfig.h"
#include "Component/VoxelResolutionComboBox.h"
#include "Windows/Window.h"

#include <string>

// Window for configuring and generating volumetric landscapes asynchronously.
class LandscapeGeneratorWindow : public Window
{
  public:
    LandscapeGeneratorWindow();
    virtual ~LandscapeGeneratorWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;
    virtual bool        isOpen() const override;
    virtual void        open() override;
    virtual void        close() override;

  private:
    void                      startGenerate();
    CADacomb::LandscapeConfig buildConfig() const;

    VoxelResolutionComboBox resolutionComboBox;

    int         resolution    = 64;
    int         octaves       = 4;
    float       frequency     = 0.05f;
    float       baseHeight    = 0.4f;
    int         seed          = 1337;
    bool        caves         = false;
    float       caveThreshold = 0.12f;
    std::string assetName     = "Landscape";
    bool        openState     = false;
};
