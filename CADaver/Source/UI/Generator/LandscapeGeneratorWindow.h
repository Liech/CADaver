#pragma once

#include "CADacomb/Volume/SVO/Generator/LandscapeConfig.h"
#include "Component/VoxelResolutionComboBox.h"

#include <string>

class GatedDatabase;

// Window for configuring and generating volumetric landscapes asynchronously with progress feedback.
class LandscapeGeneratorWindow
{
  public:
    LandscapeGeneratorWindow();
    virtual ~LandscapeGeneratorWindow();

    void render(GatedDatabase& gatedDatabase);
    void startGenerate(GatedDatabase& gatedDatabase);

    void open();
    void close();
    bool isOpen() const;
    void setOpen(bool open);

    bool isGenerating() const;

    CADacomb::LandscapeConfig getConfig() const;
    int                       getResolution() const;

  private:
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
