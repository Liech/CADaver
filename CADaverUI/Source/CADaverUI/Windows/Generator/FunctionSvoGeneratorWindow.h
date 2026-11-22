#pragma once

#include "Component/VoxelResolutionComboBox.h"
#include "Windows/Window.h"

#include <string>
#include <vector>

// Window for configuring and generating volumetric SVO assets from Lua functions with built-in sample presets.
class FunctionSvoGeneratorWindow : public Window
{
  public:
    FunctionSvoGeneratorWindow();
    virtual ~FunctionSvoGeneratorWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void               selectPreset(int presetIndex);
    int                getSelectedPresetIndex() const;
    int                getPresetCount() const;
    const std::string& getScript() const;
    void               setScript(const std::string& script);

  private:
    void startGenerate();
    void initializePresets();

    VoxelResolutionComboBox resolutionComboBox;

    std::vector<std::string> presetNames;
    std::vector<std::string> presetScripts;
    std::vector<char>        scriptBuffer;
    std::string              scriptText;
    std::string              assetName           = "FunctionSVO";
    int                      selectedPresetIndex = 0;
    int                      resolution          = 64;
};
