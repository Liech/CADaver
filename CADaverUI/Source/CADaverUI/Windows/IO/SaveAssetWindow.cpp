#include "Windows/IO/SaveAssetWindow.h"

#include "Component/SaveFileDialog.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <filesystem>
#include <imgui.h>
#include <vector>

SaveAssetWindow::SaveAssetWindow() : assetComboBox() {}

SaveAssetWindow::~SaveAssetWindow() {}

std::string SaveAssetWindow::getType() const
{
    return "SaveAsset";
}

std::string SaveAssetWindow::getDescription() const
{
    return "Window: SaveAsset\n"
           "Title: Save Asset\n"
           "Purpose: Dialog for exporting a CAD (STEP), mesh (STL), or voxel (SVO) asset from the project to disk.\n\n"
           "Parameters:\n"
           "  - Target Asset: Asset from the project to export\n"
           "  - Destination Path: Output file path on disk\n\n"
           "Dispatched Command: SaveAsset (async)";
}

void SaveAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 180.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Save Asset", &openState))
    {
        handleEscapeClose();

        assetComboBox.render(*gatedDatabase);

        std::vector<char> pathBuf(512, 0);
        size_t            pathLen = filePath.size() < pathBuf.size() - 1 ? filePath.size() : pathBuf.size() - 1;
        std::copy(filePath.begin(), filePath.begin() + pathLen, pathBuf.begin());

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f);
        if (ImGui::InputText("##SaveAssetFilePath", pathBuf.data(), pathBuf.size()))
        {
            filePath = pathBuf.data();
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse...", ImVec2(80.0f, 0.0f)))
        {
            browseFile();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canSave = !assetComboBox.getSelectedGuid().toString().empty() && !filePath.empty();
        ImGui::BeginDisabled(!canSave);
        if (ImGui::Button("Save", ImVec2(120.0f, 0.0f)))
        {
            executeSave();
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

void SaveAssetWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SaveAssetWindow::browseFile()
{
    SaveFileDialog dialog;

    std::string defaultName = "model";
    std::string assetType   = "";
    if (gatedDatabase && !assetComboBox.getSelectedGuid().toString().empty())
    {
        Polyglot::Node infoCmd = {
            {      "type",                              "GetAssetInfo" },
            {   "version",                                           1 },
            { "assetGuid", assetComboBox.getSelectedGuid().toString() }
        };
        Polyglot::Node infoRes = gatedDatabase->execute(infoCmd);
        if (infoRes["status"].asString() == "Success")
        {
            defaultName = infoRes["assetName"].asString();
            assetType   = infoRes["assetType"].asString();
        }
    }

    if (assetType == "Cad")
    {
        dialog.addFilter("STEP File", { "step", "stp" });
        dialog.setSaveFileName(defaultName + ".step");
    }
    else if (assetType == "Triangulation")
    {
        dialog.addFilter("STL File", { "stl" });
        dialog.setSaveFileName(defaultName + ".stl");
    }
    else if (assetType == "Voxel")
    {
        dialog.addFilter("SVO Voxel File", { "cadacombsvo", "svo" });
        dialog.setSaveFileName(defaultName + ".cadacombsvo");
    }
    else
    {
        dialog.addFilter("All Supported Assets", { "step", "stp", "stl", "cadacombsvo", "svo" });
        dialog.setSaveFileName(defaultName);
    }

    dialog.execute();
    if (!dialog.isCanceled())
    {
        filePath = dialog.getResultPath();
    }
}

void SaveAssetWindow::executeSave()
{
    CADacomb::GUID selectedGuid = assetComboBox.getSelectedGuid();
    if (!gatedDatabase || !defer || selectedGuid.toString().empty() || filePath.empty())
    {
        return;
    }

    std::string savePath = filePath;
    if (std::filesystem::path(savePath).extension().empty())
    {
        Polyglot::Node infoCmd = {
            {      "type",          "GetAssetInfo" },
            {   "version",                       1 },
            { "assetGuid", selectedGuid.toString() }
        };
        Polyglot::Node infoRes = gatedDatabase->execute(infoCmd);
        if (infoRes["status"].asString() == "Success")
        {
            savePath += getDefaultExtension(infoRes["assetType"].asString());
        }
    }

    GatedDatabase* db = gatedDatabase;
    defer->defer(
      [db, selectedGuid, savePath]()
      {
          Polyglot::Node command = {
              {        "type",             "SaveAsset" },
              {     "version",                       1 },
              {   "assetGuid", selectedGuid.toString() },
              { "destination",                savePath }
          };
          db->executeAsync(command);
      });
}

std::string SaveAssetWindow::getDefaultExtension(const std::string& assetType) const
{
    if (assetType == "Cad")
    {
        return ".step";
    }
    if (assetType == "Triangulation")
    {
        return ".stl";
    }
    if (assetType == "Voxel")
    {
        return ".cadacombsvo";
    }
    return "";
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SaveAssetWindow/TypeAndDefaultState")
{
    SaveAssetWindow win;
    REQUIRE(win.getType() == "SaveAsset");
    REQUIRE(!win.isOpen());
    REQUIRE(!win.getDescription().empty());

    win.open();
    REQUIRE(win.isOpen());

    win.close();
    REQUIRE(!win.isOpen());
}
#endif
