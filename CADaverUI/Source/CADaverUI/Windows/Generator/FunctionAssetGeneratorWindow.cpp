#include "Windows/Generator/FunctionAssetGeneratorWindow.h"

#include "CADacomb/Formula/FormulaLuaEmitter.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <imgui.h>

FunctionAssetGeneratorWindow::FunctionAssetGeneratorWindow()
{
    expressionBuffer.resize(65536, '\0');
    helperBuffer.resize(65536, '\0');
    resolutionComboBox.setResolution(static_cast<uint32_t>(resolution));
    initializePresets();
    selectPreset(0);
}

FunctionAssetGeneratorWindow::~FunctionAssetGeneratorWindow() {}

std::string FunctionAssetGeneratorWindow::getType() const
{
    return "FunctionAssetGenerator";
}

std::string FunctionAssetGeneratorWindow::getDescription() const
{
    return "Window: FunctionAssetGenerator\n"
           "Title: Function Asset Generator\n"
           "Purpose: Generates an SdfAsset or volumetric VoxelAsset (SVO) from a Slang-dialect Formula mapping input[0..2] in [-1, 1]^3 to signed distance output[0] (< 0 inside).\n\n"
           "Parameters:\n"
           "  - Asset Name: Name of the generated asset (default: \"FunctionAsset\")\n"
           "  - Asset Type: Target asset type (SDF or SVO)\n"
           "  - Voxel Resolution: Cubic SVO grid resolution (16 to 16384, SVO target only)\n"
           "  - Preset: Built-in sample formulas or Custom default template\n"
           "  - Helper: Optional reusable Slang helper functions\n"
           "  - Formula Expression: Slang-dialect body setting output[0] from input[0], input[1], input[2] (supports perlin3d, fbm3d)\n\n"
           "Dispatched Commands: GenerateFunctionSdf / GenerateFunctionSvo (async)";
}

void FunctionAssetGeneratorWindow::initializePresets()
{
    presetNames.clear();
    presetFormulas.clear();

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setHelper("float sdSphere(float3 p, float r)\n"
                    "{\n"
                    "    return length(p) - r;\n"
                    "}");
        f.setExpression("// input[0..2] are (x, y, z) in [-1, 1].\n"
                        "// Set output[0] < 0 inside the volume (SDF enables automatic octree pruning).\n"
                        "float3 p = float3(input[0], input[1], input[2]);\n"
                        "output[0] = sdSphere(p, 0.65);");
        presetNames.push_back("Custom (Default Template)");
        presetFormulas.push_back(f);
    }

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setExpression("float3 p = float3(input[0], input[1], input[2]);\n"
                        "output[0] = length(p) - 0.75;");
        presetNames.push_back("Sphere (SDF)");
        presetFormulas.push_back(f);
    }

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setHelper("float sdTorus(float3 p, float majorR, float minorR)\n"
                    "{\n"
                    "    float qx = sqrt(p.x * p.x + p.z * p.z) - majorR;\n"
                    "    return sqrt(qx * qx + p.y * p.y) - minorR;\n"
                    "}");
        f.setExpression("float3 p = float3(input[0], input[1], input[2]);\n"
                        "output[0] = sdTorus(p, 0.6, 0.22);");
        presetNames.push_back("Torus (SDF)");
        presetFormulas.push_back(f);
    }

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setExpression("float x = input[0];\n"
                        "float y = input[1];\n"
                        "float z = input[2];\n"
                        "float sphereDist = sqrt(x * x + y * y + z * z) - 0.85;\n"
                        "float s = 10.0;\n"
                        "float g = sin(x * s) * cos(y * s) + sin(y * s) * cos(z * s) + sin(z * s) * cos(x * s);\n"
                        "float gyroidDist = (abs(g) - 0.35) / s;\n"
                        "output[0] = max(sphereDist, gyroidDist);");
        presetNames.push_back("Gyroid Sphere");
        presetFormulas.push_back(f);
    }

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setExpression("float x = input[0];\n"
                        "float y = input[1];\n"
                        "float z = input[2];\n"
                        "float r = sqrt(x * x + y * y + z * z);\n"
                        "float disp = 0.14 * fbm3d(x, y, z, 4, 2.5, 0.5, 2.0, 42);\n"
                        "output[0] = (r - (0.65 + disp)) * 0.6;");
        presetNames.push_back("Noisy Planet (SDF + FBM)");
        presetFormulas.push_back(f);
    }

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setHelper("float sdBox(float3 p, float3 b)\n"
                    "{\n"
                    "    float3 q = abs(p) - b;\n"
                    "    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);\n"
                    "}");
        f.setExpression("float x = input[0];\n"
                        "float y = input[1];\n"
                        "float z = input[2];\n"
                        "float boxDist = sdBox(float3(x, y, z), float3(0.72, 0.72, 0.72));\n"
                        "float cylR = 0.48;\n"
                        "float cx = sqrt(y * y + z * z) - cylR;\n"
                        "float cy = sqrt(x * x + z * z) - cylR;\n"
                        "float cz = sqrt(x * x + y * y) - cylR;\n"
                        "float holes = min(cx, min(cy, cz));\n"
                        "output[0] = max(boxDist, -holes);");
        presetNames.push_back("Hollow Cube Frame (CSG)");
        presetFormulas.push_back(f);
    }

    {
        CADacomb::Formula f;
        f.setName("sdfFormula");
        f.setInputCount(3);
        f.setOutputCount(1);
        f.setExpression("float x = input[0];\n"
                        "float y = input[1];\n"
                        "float z = input[2];\n"
                        "float h = -0.15 + 0.55 * fbm3d(x, 0.0, z, 5, 1.8, 0.5, 2.0, 1337);\n"
                        "output[0] = (y - h) * 0.5;");
        presetNames.push_back("Terrain Heightfield (FBM)");
        presetFormulas.push_back(f);
    }
}

void FunctionAssetGeneratorWindow::selectPreset(int presetIndex)
{
    if (presetIndex < 0 || static_cast<size_t>(presetIndex) >= presetFormulas.size())
    {
        return;
    }

    selectedPresetIndex = presetIndex;
    setFormula(presetFormulas[static_cast<size_t>(presetIndex)]);
}

int FunctionAssetGeneratorWindow::getSelectedPresetIndex() const
{
    return selectedPresetIndex;
}

int FunctionAssetGeneratorWindow::getPresetCount() const
{
    return static_cast<int>(presetNames.size());
}

FunctionAssetTargetType FunctionAssetGeneratorWindow::getTargetType() const
{
    return targetType;
}

void FunctionAssetGeneratorWindow::setTargetType(FunctionAssetTargetType newTargetType)
{
    targetType = newTargetType;
}

const CADacomb::Formula& FunctionAssetGeneratorWindow::getFormula() const
{
    return formula;
}

void FunctionAssetGeneratorWindow::setFormula(const CADacomb::Formula& newFormula)
{
    formula = newFormula;
    syncBuffersFromFormula();
}

std::string FunctionAssetGeneratorWindow::emitLuaScript() const
{
    std::string emitted = CADacomb::FormulaLuaEmitter::emit(formula);
    emitted += "\nfunction sdf(x, y, z)\n";
    emitted += "    return " + formula.getName() + "({ x, y, z })[1]\n";
    emitted += "end\n";
    return emitted;
}

void FunctionAssetGeneratorWindow::syncBuffersFromFormula()
{
    const std::string& expr = formula.getExpression();
    if (expressionBuffer.size() <= expr.size())
    {
        expressionBuffer.resize(expr.size() + 1024, '\0');
    }
    std::memcpy(expressionBuffer.data(), expr.data(), expr.size());
    expressionBuffer[expr.size()] = '\0';

    const std::string& helper = formula.getHelper();
    if (helperBuffer.size() <= helper.size())
    {
        helperBuffer.resize(helper.size() + 1024, '\0');
    }
    std::memcpy(helperBuffer.data(), helper.data(), helper.size());
    helperBuffer[helper.size()] = '\0';
}

void FunctionAssetGeneratorWindow::syncFormulaFromBuffers()
{
    formula.setExpression(std::string(expressionBuffer.data()));
    formula.setHelper(std::string(helperBuffer.data()));
}

void FunctionAssetGeneratorWindow::startGenerate()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    syncFormulaFromBuffers();

    GatedDatabase*          db          = gatedDatabase;
    uint32_t                dim         = static_cast<uint32_t>(resolution);
    std::string             name        = assetName;
    nlohmann::json          formulaJson = formula.toJson();
    FunctionAssetTargetType target      = targetType;

    defer->defer(
      [db, dim, name, formulaJson, target]()
      {
          if (target == FunctionAssetTargetType::Sdf)
          {
              Polyglot::Node command = {
                  {      "type",       "GenerateFunctionSdf" },
                  {   "version",                           1 },
                  { "assetName",                        name },
                  {   "formula", Polyglot::Node(formulaJson) }
              };
              db->executeAsync(command);
              return;
          }

          Polyglot::Node command = {
              {      "type",       "GenerateFunctionSvo" },
              {   "version",                           1 },
              { "assetName",                        name },
              { "dimension",                         dim },
              {   "formula", Polyglot::Node(formulaJson) }
          };
          db->executeAsync(command);
      });
}

void FunctionAssetGeneratorWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(640.0f, 600.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Function Asset Generator", &openState))
    {
        handleEscapeClose();

        char nameBuffer[128];
        strncpy_s(nameBuffer, sizeof(nameBuffer), assetName.c_str(), _TRUNCATE);
        if (ImGui::InputText("Asset Name", nameBuffer, sizeof(nameBuffer)))
        {
            assetName = std::string(nameBuffer);
        }

        const char* currentTargetLabel = (targetType == FunctionAssetTargetType::Sdf) ? "SDF" : "SVO (Voxel)";
        if (ImGui::BeginCombo("Asset Type", currentTargetLabel))
        {
            bool sdfSelected = (targetType == FunctionAssetTargetType::Sdf);
            if (ImGui::Selectable("SDF", sdfSelected))
            {
                targetType = FunctionAssetTargetType::Sdf;
            }
            if (sdfSelected)
            {
                ImGui::SetItemDefaultFocus();
            }

            bool svoSelected = (targetType == FunctionAssetTargetType::Voxel);
            if (ImGui::Selectable("SVO (Voxel)", svoSelected))
            {
                targetType = FunctionAssetTargetType::Voxel;
            }
            if (svoSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (targetType == FunctionAssetTargetType::Voxel && resolutionComboBox.render())
        {
            resolution = static_cast<int>(resolutionComboBox.getResolution());
        }

        const char* currentPresetLabel = presetNames[static_cast<size_t>(selectedPresetIndex)].c_str();
        if (ImGui::BeginCombo("Sample Formula", currentPresetLabel))
        {
            for (int i = 0; i < static_cast<int>(presetNames.size()); i++)
            {
                bool isSelected = (i == selectedPresetIndex);
                if (ImGui::Selectable(presetNames[static_cast<size_t>(i)].c_str(), isSelected))
                {
                    selectPreset(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::TextWrapped("Slang Formula: input[0..2] = (x, y, z) in [-1, 1]^3, set output[0] < 0 inside (auto-pruned SDF). Built-ins: perlin3d, fbm3d.");

        if (ImGui::CollapsingHeader("Helper Functions (Slang)"))
        {
            if (ImGui::InputTextMultiline("##FormulaHelper", helperBuffer.data(), helperBuffer.size(), ImVec2(-1.0f, 120.0f), ImGuiInputTextFlags_AllowTabInput))
            {
                formula.setHelper(std::string(helperBuffer.data()));
            }
        }

        ImGui::TextUnformatted("Expression (Slang):");
        ImVec2 editorSize(-1.0f, ImGui::GetContentRegionAvail().y - 44.0f);
        if (editorSize.y < 140.0f)
        {
            editorSize.y = 140.0f;
        }

        if (ImGui::InputTextMultiline("##FormulaExpression", expressionBuffer.data(), expressionBuffer.size(), editorSize, ImGuiInputTextFlags_AllowTabInput))
        {
            formula.setExpression(std::string(expressionBuffer.data()));
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        const char* buttonLabel = (targetType == FunctionAssetTargetType::Sdf) ? "Generate SDF" : "Generate SVO";
        if (ImGui::Button(buttonLabel, ImVec2(180.0f, 0.0f)))
        {
            startGenerate();
            close();
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include "Utility/LuaSVOField.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("FunctionAssetGeneratorWindow/InitialStateAndTargetSelection")
{
    FunctionAssetGeneratorWindow window;
    CHECK(window.getType() == "FunctionAssetGenerator");
    CHECK_FALSE(window.getDescription().empty());
    CHECK_FALSE(window.isOpen());
    CHECK(window.getTargetType() == FunctionAssetTargetType::Sdf);
    CHECK(window.getPresetCount() >= 5);
    CHECK(window.getSelectedPresetIndex() == 0);
    CHECK_FALSE(window.getFormula().getExpression().empty());

    window.setTargetType(FunctionAssetTargetType::Voxel);
    CHECK(window.getTargetType() == FunctionAssetTargetType::Voxel);

    window.selectPreset(2);
    CHECK(window.getSelectedPresetIndex() == 2);
    CHECK(window.getFormula().getHelper().find("sdTorus") != std::string::npos);

    window.open();
    CHECK(window.isOpen());

    window.close();
    CHECK_FALSE(window.isOpen());
}

TEST_CASE("FunctionAssetGeneratorWindow/AllPresetsGenerateValidSVO")
{
    FunctionAssetGeneratorWindow window;
    for (int i = 0; i < window.getPresetCount(); i++)
    {
        window.selectPreset(i);
        std::string error;
        auto        svo = LuaSVOField::generateSVO(16, window.emitLuaScript(), error);
        REQUIRE(svo != nullptr);
        CHECK(error.empty());
    }
}
#endif
