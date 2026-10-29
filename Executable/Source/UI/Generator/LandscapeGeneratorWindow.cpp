#include "UI/Generator/LandscapeGeneratorWindow.h"

#include "CADacomb/Util/ProgressBar.h"
#include "CADacomb/Volume/MergeStrategy/MostPrevalentExceptMergeStrategy.h"
#include "CADacomb/Volume/SVO/Generator/SVOLandscapeGenerator.h"
#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Commands/Undo/AddAssetUndoRecord.h"
#include "Database/Asset/VoxelAsset.h"

#include <chrono>
#include <imgui.h>

LandscapeGeneratorWindow::LandscapeGeneratorWindow()
{
    resolutionComboBox.setResolution(static_cast<uint32_t>(resolution));
}

LandscapeGeneratorWindow::~LandscapeGeneratorWindow()
{
    if (workerThread && workerThread->joinable())
    {
        CADacomb::ProgressBar::cancel(taskGuid);
        workerThread->join();
    }
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

void LandscapeGeneratorWindow::setOpen(bool open)
{
    openState = open;
}

bool LandscapeGeneratorWindow::isGenerating() const
{
    return generatingState.load() || (workerThread && workerThread->joinable());
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
    if (generatingState.load())
    {
        return;
    }

    if (workerThread && workerThread->joinable())
    {
        workerThread->join();
    }

    hasNewSVO       = false;
    generatingState = true;
    modalProgressBar.reset();

    taskGuid = CADacomb::GUID::random();
    modalProgressBar.setGuid(taskGuid);

    auto        config  = getConfig();
    int         res     = resolution;
    std::string name    = assetName;
    auto        guidVal = taskGuid;

    workerThread = std::make_unique<std::thread>(
      [this, res, config, name, guidVal]()
      {
          CADacomb::ProgressBar::setThreadGuid(guidVal);
          auto t0 = std::chrono::high_resolution_clock::now();

          auto svo = CADacomb::SVOLandscapeGenerator::generateSVO<bool>(
            static_cast<uint32_t>(res), config, glm::dvec3(0.0), true, false, 32, CADacomb::MostPrevalentExceptMergeStrategy<bool, false>::instance());

          auto t1 = std::chrono::high_resolution_clock::now();

          {
              std::lock_guard<std::mutex> lock(resultMutex);
              pendingSVO       = std::move(svo);
              generationTimeMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
              nodeCount        = pendingSVO ? pendingSVO->getRaw().size() : 0;
          }

          generatingState.store(false);
      });
}

void LandscapeGeneratorWindow::commitAsset(std::unique_ptr<CADacomb::SVO<bool>> svo)
{
    auto        svoHolder = std::make_shared<std::unique_ptr<CADacomb::SVO<bool>>>(std::move(svo));
    std::string name      = assetName;
    uint32_t    dim       = static_cast<uint32_t>(resolution);

    Defer::getInstance().defer(
      [svoHolder, name, dim]()
      {
          if (*svoHolder)
          {
              auto voxelAsset = std::make_shared<VoxelAsset>(name, std::move(*svoHolder), CADacomb::AABB3(glm::dvec3(0.0), glm::dvec3(static_cast<double>(dim))));

              auto& gatedDb = GatedDatabase::getInstance();
              const_cast<Database&>(gatedDb.get()).getProject().addAsset(voxelAsset);
              const_cast<Database&>(gatedDb.get()).getProject().setSelectedAssetGuid(voxelAsset->getGuid());
              const_cast<Database&>(gatedDb.get()).getUndoStack().push(std::make_unique<AddAssetUndoRecord>(voxelAsset));
          }
      });
}

void LandscapeGeneratorWindow::render()
{
    if (!openState && !isGenerating())
    {
        return;
    }

    if (isGenerating())
    {
        modalProgressBar.render("Generating Landscape");

        if (!generatingState.load())
        {
            if (workerThread && workerThread->joinable())
            {
                workerThread->join();
                workerThread.reset();
            }
            modalProgressBar.close();

            std::unique_ptr<CADacomb::SVO<bool>> readySVO;
            {
                std::lock_guard<std::mutex> lock(resultMutex);
                readySVO = std::move(pendingSVO);
            }

            if (readySVO)
            {
                commitAsset(std::move(readySVO));
                hasNewSVO = true;
            }
        }
    }

    if (!openState)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 460.0f), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Landscape Generator", &openState))
    {
        bool disabled = isGenerating();
        if (disabled)
        {
            ImGui::BeginDisabled();
        }

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

        if (ImGui::Button("Generate Landscape", ImVec2(180.0f, 0.0f)) && !isGenerating())
        {
            startGenerate();
        }

        if (disabled)
        {
            ImGui::EndDisabled();
        }

        if (hasNewSVO)
        {
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Generated in %.1f ms (%zu octree nodes)", generationTimeMs, nodeCount);
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
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
