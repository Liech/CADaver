#include "Windows/IO/SaveStepAssetWindow.h"

#include "Database/Asset.h"
#include "Database/Database.h"
#include "Database/Project.h"
#include "Dialog/SaveFileDialog.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <filesystem>
#include <imgui.h>

SaveStepAssetWindow::SaveStepAssetWindow()
  : assetComboBox("Cad")
{
}

SaveStepAssetWindow::~SaveStepAssetWindow() {}

std::string SaveStepAssetWindow::getType() const
{
    return "SaveStepAssetWindow";
}

std::string SaveStepAssetWindow::getDescription() const
{
    return "Window: SaveStepAssetWindow\n"
           "Title: Save STEP Asset\n"
           "Purpose: Dialog for exporting a CAD asset from the project to a STEP (.step / .stp) file on disk.\n\n"
           "Parameters:\n"
           "  - Target Asset: CAD asset from the project to export\n"
           "  - File Path: Destination .step or .stp file path on disk\n\n"
           "Dispatched Command: SaveStepAsset (async)";
}

void SaveStepAssetWindow::setTargetAsset(const CADacomb::GUID& assetGuid)
{
    assetComboBox.setSelectedGuid(assetGuid);
}

void SaveStepAssetWindow::browseFile(const Project& project)
{
    SaveFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });

    std::string defaultName = "model.step";
    auto        asset       = project.getAsset(assetComboBox.getSelectedGuid());
    if (asset)
    {
        defaultName = asset->getName();
        if (std::filesystem::path(defaultName).extension().empty())
        {
            defaultName += ".step";
        }
    }
    dialog.setSaveFileName(defaultName);
    dialog.execute();

    if (!dialog.isCanceled() && !dialog.getResultPath().empty())
    {
        filePath = dialog.getResultPath();
        if (std::filesystem::path(filePath).extension().empty())
        {
            filePath += ".step";
        }
    }
}

void SaveStepAssetWindow::executeSave()
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
        savePath += ".step";
    }

    defer->defer(
      [db, guid, savePath]()
      {
          db->executeAsync({
            {      "type", "SaveStepAsset" },
            {   "version",               1 },
            { "assetGuid", guid.toString() },
            {  "filePath",        savePath }
          });
      });
}

void SaveStepAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Save STEP Asset##SaveStepWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
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
