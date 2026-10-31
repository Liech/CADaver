#include "Windows/Generator/LandscapeGeneratorWindow.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <imgui.h>

LandscapeGeneratorWindow::LandscapeGeneratorWindow()
{
    resolutionComboBox.setResolution(static_cast<uint32_t>(resolution));
}

LandscapeGeneratorWindow::~LandscapeGeneratorWindow() {}

std::string LandscapeGeneratorWindow::getType() const
{
    return "LandscapeGeneratorWindow";
}

std::string LandscapeGeneratorWindow::getDescription() const
{
    return "Window: LandscapeGeneratorWindow\n"
           "Title: Landscape Generator\n"
           "Purpose: Configures procedural noise parameters and generates a volumetric SVO terrain asset asynchronously.\n\n"
           "Parameters:\n"
           "  - Asset Name: Name of the generated VoxelAsset (default: \"Landscape\")\n"
           "  - Voxel Resolution: Cubic SVO grid resolution (16, 32, 64, 128, 256, 512)\n"
           "  - Octaves: Number of fractal noise octaves (1 to 8)\n"
           "  - Frequency: Base terrain noise frequency (0.01 to 0.2)\n"
           "  - Base Height: Normalized terrain base height ratio (0.1 to 0.9)\n"
           "  - Seed: Random number generator seed\n"
           "  - Carve Caves: Enables 3D cave carving inside the terrain volume\n"
           "  - Cave Threshold: Density threshold for cave carving (0.01 to 0.3)\n\n"
           "Dispatched Command: GenerateLandscape (async)";
}

void LandscapeGeneratorWindow::open()
{
    openState = true;
}

void LandscapeGeneratorWindow::close()
{
    openState = false;
}

bool LandscapeGeneratorWindow::isOpen() const
{
    return openState;
}

CADacomb::LandscapeConfig LandscapeGeneratorWindow::buildConfig() const
{
    CADacomb::LandscapeConfig config;
    config.seed            = static_cast<uint32_t>(seed);
    config.octaves         = octaves;
    config.frequency       = static_cast<double>(frequency);
    config.baseHeightRatio = static_cast<double>(baseHeight);
    config.enableCaves     = caves;
    config.caveThreshold   = static_cast<double>(caveThreshold);
    return config;
}

void LandscapeGeneratorWindow::startGenerate()
{
    if (!gatedDatabase || !defer)
    {
        return;
    }

    GatedDatabase* db     = gatedDatabase;
    auto           config = buildConfig();
    uint32_t       dim    = static_cast<uint32_t>(resolution);
    std::string    name   = assetName;

    defer->defer(
      [db, config, dim, name]()
      {
          nlohmann::json cmdJson = {
              {            "type",    "GenerateLandscape" },
              {         "version",                      1 },
              {       "assetName",                   name },
              {       "dimension",                    dim },
              {            "seed",            config.seed },
              {         "octaves",         config.octaves },
              {       "frequency",       config.frequency },
              {     "persistence",     config.persistence },
              {      "lacunarity",      config.lacunarity },
              { "baseHeightRatio", config.baseHeightRatio },
              {     "heightScale",     config.heightScale },
              {     "enableCaves",     config.enableCaves },
              {   "caveFrequency",   config.caveFrequency },
              {   "caveThreshold",   config.caveThreshold },
              {  "solidThreshold",  config.solidThreshold }
          };
          db->executeAsync(cmdJson);
      });
}

void LandscapeGeneratorWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 460.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Landscape Generator", &openState))
    {
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

        ImGui::SliderInt("Octaves", &octaves, 1, 8);
        ImGui::SliderFloat("Frequency", &frequency, 0.01f, 0.2f, "%.4f");
        ImGui::SliderFloat("Base Height", &baseHeight, 0.1f, 0.9f, "%.2f");
        ImGui::InputInt("Seed", &seed);
        ImGui::Checkbox("Carve Caves", &caves);
        if (caves)
        {
            ImGui::SliderFloat("Cave Threshold", &caveThreshold, 0.01f, 0.3f, "%.3f");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Generate Landscape", ImVec2(180.0f, 0.0f)))
        {
            startGenerate();
            close();
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LandscapeGeneratorWindow/InitialState")
{
    LandscapeGeneratorWindow window;
    CHECK(window.getType() == "LandscapeGeneratorWindow");
    CHECK_FALSE(window.getDescription().empty());
    CHECK_FALSE(window.isOpen());

    window.open();
    CHECK(window.isOpen());

    window.close();
    CHECK_FALSE(window.isOpen());
}
#endif
