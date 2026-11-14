#include "Windows/IO/LoadAssetWindow.h"

#include "Component/LoadFileDialog.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <filesystem>
#include <imgui.h>
#include <vector>

LoadAssetWindow::LoadAssetWindow() {}

LoadAssetWindow::~LoadAssetWindow() {}

std::string LoadAssetWindow::getType() const
{
    return "LoadAsset";
}

std::string LoadAssetWindow::getDescription() const
{
    return "Window: LoadAsset\n"
           "Title: Load Asset\n"
           "Purpose: Dialog for importing a CAD (STEP), mesh (STL), or voxel (SVO) asset file from disk into the project.\n\n"
           "Parameters:\n"
           "  - File Path: Path to the .step, .stp, .stl, .cadacombsvo, or .svo file on disk\n"
           "  - Asset Name: Name assigned to the loaded asset (defaults to file stem)\n\n"
           "Dispatched Command: LoadAsset (async)";
}

void LoadAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 180.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Load Asset", &openState))
    {
        handleEscapeClose();

        std::vector<char> pathBuf(512, 0);
        size_t            pathLen = filePath.size() < pathBuf.size() - 1 ? filePath.size() : pathBuf.size() - 1;
        std::copy(filePath.begin(), filePath.begin() + pathLen, pathBuf.begin());

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
        if (ImGui::InputText("##AssetFilePath", pathBuf.data(), pathBuf.size()))
        {
            filePath = pathBuf.data();
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse...", ImVec2(80.0f, 0.0f)))
        {
            browseFile();
        }

        std::vector<char> nameBuf(256, 0);
        size_t            nameLen = assetName.size() < nameBuf.size() - 1 ? assetName.size() : nameBuf.size() - 1;
        std::copy(assetName.begin(), assetName.begin() + nameLen, nameBuf.begin());

        if (ImGui::InputText("Asset Name", nameBuf.data(), nameBuf.size()))
        {
            assetName = nameBuf.data();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canLoad = !filePath.empty() && std::filesystem::exists(filePath);
        ImGui::BeginDisabled(!canLoad);
        if (ImGui::Button("Load", ImVec2(120.0f, 0.0f)))
        {
            executeLoad();
            close();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}

void LoadAssetWindow::browseFile()
{
    LoadFileDialog dialog;
    dialog.addFilter("All Supported Assets", { "step", "stp", "stl", "cadacombsvo", "svo" });
    dialog.addFilter("STEP File", { "step", "stp" });
    dialog.addFilter("STL File", { "stl" });
    dialog.addFilter("SVO Voxel File", { "cadacombsvo", "svo" });
    dialog.execute();

    if (!dialog.isCanceled())
    {
        filePath  = dialog.getResultPath();
        assetName = std::filesystem::path(filePath).stem().string();
    }
}

void LoadAssetWindow::executeLoad()
{
    if (!gatedDatabase || !defer || filePath.empty())
    {
        return;
    }

    GatedDatabase* db        = gatedDatabase;
    std::string    source    = filePath;
    std::string    finalName = assetName.empty() ? std::filesystem::path(filePath).stem().string() : assetName;

    defer->defer(
      [db, finalName, source]()
      {
          Polyglot::Node command = {
              {    "type", "LoadAsset" },
              { "version",           1 },
              {    "name",   finalName },
              {  "source",      source }
          };
          db->executeAsync(command);
      });
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadAssetWindow/TypeAndDefaultState")
{
    LoadAssetWindow win;
    REQUIRE(win.getType() == "LoadAsset");
    REQUIRE(!win.isOpen());
    REQUIRE(!win.getDescription().empty());

    win.open();
    REQUIRE(win.isOpen());

    win.close();
    REQUIRE(!win.isOpen());
}
#endif
