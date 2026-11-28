#pragma once

#include "CADacomb/Formula/Formula.h"
#include "Component/VoxelResolutionComboBox.h"
#include "Windows/Generator/FunctionAssetTargetType.h"
#include "Windows/Window.h"

#include <string>
#include <vector>

// Window for generating SDF or SVO assets from Slang-dialect Formula presets and custom expressions.
class FunctionAssetGeneratorWindow : public Window
{
  public:
    FunctionAssetGeneratorWindow();
    virtual ~FunctionAssetGeneratorWindow();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        render() override;

    void                     selectPreset(int presetIndex);
    int                      getSelectedPresetIndex() const;
    int                      getPresetCount() const;
    FunctionAssetTargetType  getTargetType() const;
    void                     setTargetType(FunctionAssetTargetType targetType);
    const CADacomb::Formula& getFormula() const;
    void                     setFormula(const CADacomb::Formula& formula);
    std::string              emitLuaScript() const;

  private:
    void startGenerate();
    void initializePresets();
    void syncBuffersFromFormula();
    void syncFormulaFromBuffers();

    VoxelResolutionComboBox resolutionComboBox;

    std::vector<std::string>       presetNames;
    std::vector<CADacomb::Formula> presetFormulas;
    CADacomb::Formula              formula;
    std::vector<char>              expressionBuffer;
    std::vector<char>              helperBuffer;
    std::string                    assetName           = "FunctionAsset";
    FunctionAssetTargetType        targetType          = FunctionAssetTargetType::Sdf;
    int                            selectedPresetIndex = 0;
    int                            resolution          = 64;
};
