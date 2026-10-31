#include "Windows/IO/SaveVoxelAssetWindow.h"

#include "Component/SaveFileDialog.h"
#include "Database/Asset.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <memory>

SaveVoxelAssetWindow::SaveVoxelAssetWindow()
  : assetComboBox("Voxel")
{
}

SaveVoxelAssetWindow::~SaveVoxelAssetWindow() {}

std::string SaveVoxelAssetWindow::getType() const
{
    return "SaveVoxelAssetWindow";
}

std::string SaveVoxelAssetWindow::getDescription() const
{
    return "Window: SaveVoxelAssetWindow\n"
           "Title: Save SVO Asset\n"
           "Purpose: Dialog for exporting a Sparse Voxel Octree (SVO) volumetric asset from the project to a .cadacombsvo or .svo file on disk.\n\n"
           "Parameters:\n"
           "  - Target Asset: Voxel asset from the project to export\n"
           "  - File Path: Destination .cadacombsvo or .svo file path on disk\n\n"
           "Dispatched Command: SaveVoxelAsset (async)";
}

void SaveVoxelAssetWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SaveVoxelAssetWindow::browseFile(const Project& project)
{
    SaveFileDialog dialog;
    dialog.addFilter("SVO File", { "cadacombsvo", "svo" });

    std::string defaultName = "volume.cadacombsvo";
    auto        asset       = project.getAsset(assetComboBox.getSelectedGuid());
    if (asset)
    {
        defaultName = asset->getName();
        if (std::filesystem::path(defaultName).extension().empty())
        {
            defaultName += ".cadacombsvo";
        }
    }
    dialog.setSaveFileName(defaultName);
    dialog.execute();

    if (!dialog.isCanceled() && !dialog.getResultPath().empty())
    {
        filePath = dialog.getResultPath();
        if (std::filesystem::path(filePath).extension().empty())
        {
            filePath += ".cadacombsvo";
        }
    }
}

void SaveVoxelAssetWindow::executeSave()
{
    if (!gatedDatabase || !defer || filePath.empty())
    {
        return;
    }

    GatedDatabase* db       = gatedDatabase;
    CADacomb::GUID guid     = assetComboBox.getSelectedGuid();
    std::string    savePath = filePath;
    if (std::filesystem::path(savePath).extension().empty())
    {
        savePath += ".cadacombsvo";
    }

    defer->defer(
      [db, guid, savePath]()
      {
          auto fileStream = std::make_shared<std::ofstream>(savePath, std::ios::binary);
          db->executeAsync(
            {
              {      "type", "SaveVoxelAsset" },
              {   "version",                1 },
              { "assetGuid",  guid.toString() }
          },
            nullptr,
            fileStream);
      });
}

void SaveVoxelAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Save SVO Asset##SaveSvoWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        const auto& project = gatedDatabase->get().getProject();
        assetComboBox.render(project);
        ImGui::Spacing();

        char pathBuffer[512];
        strncpy_s(pathBuffer, sizeof(pathBuffer), filePath.c_str(), _TRUNCATE);
        if (ImGui::InputText("File Path", pathBuffer, sizeof(pathBuffer)))
        {
            filePath = std::string(pathBuffer);
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            browseFile(project);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        bool canSave = project.hasAsset(assetComboBox.getSelectedGuid()) && !filePath.empty();
        ImGui::BeginDisabled(!canSave);
        if (ImGui::Button("Save", ImVec2(120.0f, 0.0f)))
        {
            executeSave();
            close();
        }
        ImGui::EndDisabled();

        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            close();
        }
    }
    ImGui::End();
}
