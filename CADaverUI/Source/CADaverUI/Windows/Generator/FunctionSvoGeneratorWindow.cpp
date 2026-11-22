#include "Windows/Generator/FunctionSvoGeneratorWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <imgui.h>

FunctionSvoGeneratorWindow::FunctionSvoGeneratorWindow()
{
    scriptBuffer.resize(65536, '\0');
    resolutionComboBox.setResolution(static_cast<uint32_t>(resolution));
    initializePresets();
    selectPreset(0);
}

FunctionSvoGeneratorWindow::~FunctionSvoGeneratorWindow() {}

std::string FunctionSvoGeneratorWindow::getType() const
{
    return "FunctionSvoGenerator";
}

std::string FunctionSvoGeneratorWindow::getDescription() const
{
    return "Window: FunctionSvoGenerator\n"
           "Title: Function SVO Generator\n"
           "Purpose: Generates a volumetric SVO asset from a Lua script defining sdf(x, y, z) or eval(x, y, z) over [-1, 1]^3.\n\n"
           "Parameters:\n"
           "  - Asset Name: Name of the generated VoxelAsset (default: \"FunctionSVO\")\n"
           "  - Voxel Resolution: Cubic SVO grid resolution (16 to 16384)\n"
           "  - Preset: Built-in sample functions or Custom default template\n"
           "  - Lua Script: Editable Lua code supporting sdf(x, y, z), eval(x, y, z), classifyBox(minX, minY, minZ, size), perlin3d, and fbm3d\n\n"
           "Dispatched Command: GenerateFunctionSvo (async)";
}

void FunctionSvoGeneratorWindow::initializePresets()
{
    presetNames.clear();
    presetScripts.clear();

    presetNames.push_back("Custom (Default Template)");
    presetScripts.push_back("-- Coordinates (x, y, z) are in [-1, 1].\n"
                            "-- Option A (fastest, automatic octree pruning): define sdf(x, y, z) < 0 inside.\n"
                            "-- Option B (general): define eval(x, y, z) returning true/false or density > 0.\n"
                            "-- Built-in helpers: perlin3d(x, y, z, seed), fbm3d(x, y, z, octaves, freq, persistence, lacunarity, seed)\n\n"
                            "function sdf(x, y, z)\n"
                            "    local r = math.sqrt(x * x + y * y + z * z)\n"
                            "    return r - 0.65\n"
                            "end\n");

    presetNames.push_back("Sphere (SDF)");
    presetScripts.push_back("function sdf(x, y, z)\n"
                            "    return math.sqrt(x * x + y * y + z * z) - 0.75\n"
                            "end\n");

    presetNames.push_back("Torus (SDF)");
    presetScripts.push_back("function sdf(x, y, z)\n"
                            "    local majorR = 0.6\n"
                            "    local minorR = 0.22\n"
                            "    local qx = math.sqrt(x * x + z * z) - majorR\n"
                            "    return math.sqrt(qx * qx + y * y) - minorR\n"
                            "end\n");

    presetNames.push_back("Gyroid Sphere");
    presetScripts.push_back("function eval(x, y, z)\n"
                            "    local r2 = x * x + y * y + z * z\n"
                            "    if r2 > 0.85 * 0.85 then\n"
                            "        return false\n"
                            "    end\n"
                            "    local s = 10.0\n"
                            "    local g = math.sin(x * s) * math.cos(y * s) + math.sin(y * s) * math.cos(z * s) + math.sin(z * s) * math.cos(x * s)\n"
                            "    return math.abs(g) < 0.35\n"
                            "end\n\n"
                            "function classifyBox(minX, minY, minZ, size)\n"
                            "    local cx = minX + 0.5 * size\n"
                            "    local cy = minY + 0.5 * size\n"
                            "    local cz = minZ + 0.5 * size\n"
                            "    local radius = size * 0.8660254\n"
                            "    if math.sqrt(cx * cx + cy * cy + cz * cz) - radius > 0.85 then\n"
                            "        return false\n"
                            "    end\n"
                            "    return nil\n"
                            "end\n");

    presetNames.push_back("Noisy Planet (SDF + FBM)");
    presetScripts.push_back("function sdf(x, y, z)\n"
                            "    local r = math.sqrt(x * x + y * y + z * z)\n"
                            "    local disp = 0.14 * fbm3d(x, y, z, 4, 2.5, 0.5, 2.0, 42)\n"
                            "    return (r - (0.65 + disp)) * 0.6\n"
                            "end\n");

    presetNames.push_back("Hollow Cube Frame (CSG)");
    presetScripts.push_back("function sdf(x, y, z)\n"
                            "    local ax = math.abs(x) - 0.72\n"
                            "    local ay = math.abs(y) - 0.72\n"
                            "    local az = math.abs(z) - 0.72\n"
                            "    local outDist = math.sqrt(math.max(ax, 0)^2 + math.max(ay, 0)^2 + math.max(az, 0)^2)\n"
                            "    local inDist = math.min(math.max(ax, math.max(ay, az)), 0.0)\n"
                            "    local boxDist = outDist + inDist\n\n"
                            "    local cylR = 0.48\n"
                            "    local cx = math.sqrt(y * y + z * z) - cylR\n"
                            "    local cy = math.sqrt(x * x + z * z) - cylR\n"
                            "    local cz = math.sqrt(x * x + y * y) - cylR\n"
                            "    local holes = math.min(cx, math.min(cy, cz))\n"
                            "    return math.max(boxDist, -holes)\n"
                            "end\n");

    presetNames.push_back("Terrain Heightfield (FBM)");
    presetScripts.push_back("function eval(x, y, z)\n"
                            "    local h = -0.15 + 0.55 * fbm3d(x, 0.0, z, 5, 1.8, 0.5, 2.0, 1337)\n"
                            "    return y < h\n"
                            "end\n\n"
                            "function classifyBox(minX, minY, minZ, size)\n"
                            "    if minY > 0.45 then\n"
                            "        return false\n"
                            "    end\n"
                            "    if minY + size < -0.75 then\n"
                            "        return true\n"
                            "    end\n"
                            "    return nil\n"
                            "end\n");
}

void FunctionSvoGeneratorWindow::selectPreset(int presetIndex)
{
    if (presetIndex < 0 || static_cast<size_t>(presetIndex) >= presetScripts.size())
    {
        return;
    }

    selectedPresetIndex = presetIndex;
    setScript(presetScripts[static_cast<size_t>(presetIndex)]);
}

int FunctionSvoGeneratorWindow::getSelectedPresetIndex() const
{
    return selectedPresetIndex;
}

int FunctionSvoGeneratorWindow::getPresetCount() const
{
    return static_cast<int>(presetNames.size());
}

const std::string& FunctionSvoGeneratorWindow::getScript() const
{
    return scriptText;
}

void FunctionSvoGeneratorWindow::setScript(const std::string& script)
{
    scriptText = script;
    if (scriptBuffer.size() <= script.size())
    {
        scriptBuffer.resize(script.size() + 1024, '\0');
    }

    std::memcpy(scriptBuffer.data(), script.data(), script.size());
    scriptBuffer[script.size()] = '\0';
}

void FunctionSvoGeneratorWindow::startGenerate()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    scriptText = std::string(scriptBuffer.data());

    GatedDatabase* db     = gatedDatabase;
    uint32_t       dim    = static_cast<uint32_t>(resolution);
    std::string    name   = assetName;
    std::string    script = scriptText;

    defer->defer(
      [db, dim, name, script]()
      {
          Polyglot::Node command = {
              {      "type", "GenerateFunctionSvo" },
              {   "version",                     1 },
              { "assetName",                  name },
              { "dimension",                   dim },
              {    "script",                script }
          };
          db->executeAsync(command);
      });
}

void FunctionSvoGeneratorWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(640.0f, 540.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Function SVO Generator", &openState))
    {
        handleEscapeClose();

        char nameBuffer[128];
        strncpy_s(nameBuffer, sizeof(nameBuffer), assetName.c_str(), _TRUNCATE);
        if (ImGui::InputText("Asset Name", nameBuffer, sizeof(nameBuffer)))
        {
            assetName = std::string(nameBuffer);
        }

        if (resolutionComboBox.render())
        {
            resolution = static_cast<int>(resolutionComboBox.getResolution());
        }

        const char* currentPresetLabel = presetNames[static_cast<size_t>(selectedPresetIndex)].c_str();
        if (ImGui::BeginCombo("Sample Function", currentPresetLabel))
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

        ImGui::TextWrapped("Define sdf(x, y, z) (< 0 inside, auto-pruned) or eval(x, y, z) (bool or > 0) over [-1, 1]^3. Helpers: perlin3d, fbm3d.");

        ImVec2 editorSize(-1.0f, ImGui::GetContentRegionAvail().y - 44.0f);
        if (editorSize.y < 140.0f)
        {
            editorSize.y = 140.0f;
        }

        if (ImGui::InputTextMultiline("##LuaFunctionScript", scriptBuffer.data(), scriptBuffer.size(), editorSize, ImGuiInputTextFlags_AllowTabInput))
        {
            scriptText = std::string(scriptBuffer.data());
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Generate SVO", ImVec2(180.0f, 0.0f)))
        {
            startGenerate();
            close();
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FunctionSvoGeneratorWindow/InitialStateAndPresets")
{
    FunctionSvoGeneratorWindow window;
    CHECK(window.getType() == "FunctionSvoGenerator");
    CHECK_FALSE(window.getDescription().empty());
    CHECK_FALSE(window.isOpen());
    CHECK(window.getPresetCount() >= 5);
    CHECK(window.getSelectedPresetIndex() == 0);
    CHECK_FALSE(window.getScript().empty());

    window.selectPreset(2);
    CHECK(window.getSelectedPresetIndex() == 2);
    CHECK(window.getScript().find("sdf") != std::string::npos);

    window.open();
    CHECK(window.isOpen());

    window.close();
    CHECK_FALSE(window.isOpen());
}
#endif
