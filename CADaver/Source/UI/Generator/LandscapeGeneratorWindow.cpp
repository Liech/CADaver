#include "UI/Generator/LandscapeGeneratorWindow.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"

#include <imgui.h>

LandscapeGeneratorWindow::LandscapeGeneratorWindow()
{
    resolutionComboBox.setResolution(static_cast<uint32_t>(resolution));
}

LandscapeGeneratorWindow::~LandscapeGeneratorWindow() {}

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

void LandscapeGeneratorWindow::setOpen(bool open)
{
    openState = open;
}

bool LandscapeGeneratorWindow::isGenerating() const
{
    return false;
}

CADacomb::LandscapeConfig LandscapeGeneratorWindow::getConfig() const
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

int LandscapeGeneratorWindow::getResolution() const
{
    return resolution;
}

void LandscapeGeneratorWindow::startGenerate()
{
    auto        config = getConfig();
    uint32_t    dim    = static_cast<uint32_t>(resolution);
    std::string name   = assetName;

    Defer::getInstance().defer(
      [config, dim, name]()
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
          GatedDatabase::getInstance().executeAsync(cmdJson, true);
      });
}

void LandscapeGeneratorWindow::render()
{
    if (!openState)
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
            resolution = resolutionComboBox.getResolution();
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
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LandscapeGeneratorWindow/InitialState")
{
    LandscapeGeneratorWindow window;
    CHECK_FALSE(window.isOpen());
    CHECK_FALSE(window.isGenerating());
    CHECK(window.getResolution() == 64);

    window.open();
    CHECK(window.isOpen());

    window.close();
    CHECK_FALSE(window.isOpen());
}

TEST_CASE("LandscapeGeneratorWindow/ConfigValues")
{
    LandscapeGeneratorWindow window;
    auto                     config = window.getConfig();
    CHECK(config.seed == 1337);
    CHECK(config.octaves == 4);
    CHECK(config.frequency == Catch::Approx(0.05));
    CHECK(config.baseHeightRatio == Catch::Approx(0.4));
    CHECK_FALSE(config.enableCaves);
}
#endif
