#include "Windows/IO/SaveStlAssetWindow.h"

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

SaveStlAssetWindow::SaveStlAssetWindow()
  : assetComboBox("Triangulation")
{
}

SaveStlAssetWindow::~SaveStlAssetWindow() {}

std::string SaveStlAssetWindow::getType() const
{
    return "SaveStlAsset";
}

std::string SaveStlAssetWindow::getDescription() const
{
    return "Window: SaveStlAsset\n"
           "Title: Save STL Asset\n"
           "Purpose: Dialog for exporting a triangulated mesh asset from the project to an STL (.stl) file on disk.\n\n"
           "Parameters:\n"
           "  - Target Asset: Triangulation asset from the project to export\n"
           "  - File Path: Destination .stl file path on disk\n\n"
           "Dispatched Command: SaveStlAsset (async)";
}

void SaveStlAssetWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SaveStlAssetWindow::browseFile(const Project& project)
{
    SaveFileDialog dialog;
    dialog.addFilter("STL File", { "stl" });

    std::string defaultName = "mesh.stl";
    auto        asset       = project.getAsset(assetComboBox.getSelectedGuid());
    if (asset)
    {
        defaultName = asset->getName();
        if (std::filesystem::path(defaultName).extension().empty())
        {
            defaultName += ".stl";
        }
    }
    dialog.setSaveFileName(defaultName);
    dialog.execute();

    if (!dialog.isCanceled() && !dialog.getResultPath().empty())
    {
        filePath = dialog.getResultPath();
        if (std::filesystem::path(filePath).extension().empty())
        {
            filePath += ".stl";
        }
    }
}

void SaveStlAssetWindow::executeSave()
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
        savePath += ".stl";
    }

    defer->defer(
      [db, guid, savePath]()
      {
          auto fileStream = std::make_shared<std::ofstream>(savePath, std::ios::binary);
          db->executeAsync(
            {
              {      "type",  "SaveStlAsset" },
              {   "version",               1 },
              { "assetGuid", guid.toString() }
          },
            nullptr,
            fileStream);
      });
}

void SaveStlAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Save STL Asset##SaveStlWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        handleEscapeClose();
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
