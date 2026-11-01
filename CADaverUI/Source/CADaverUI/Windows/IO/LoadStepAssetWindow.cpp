#include "Windows/IO/LoadStepAssetWindow.h"

#include "Component/LoadFileDialog.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <filesystem>
#include <imgui.h>

LoadStepAssetWindow::LoadStepAssetWindow() {}

LoadStepAssetWindow::~LoadStepAssetWindow() {}

std::string LoadStepAssetWindow::getType() const
{
    return "LoadStepAsset";
}

std::string LoadStepAssetWindow::getDescription() const
{
    return "Window: LoadStepAsset\n"
           "Title: Load STEP Asset\n"
           "Purpose: Dialog for selecting a STEP/STP file on disk and importing it into the project as a CadAsset.\n\n"
           "Parameters:\n"
           "  - File Path: Path to the source .step or .stp file on disk\n"
           "  - Asset Name: Display name for the loaded CAD asset in the project\n\n"
           "Dispatched Command: LoadStepAsset (async)";
}

void LoadStepAssetWindow::browseFile()
{
    LoadFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });
    dialog.execute();

    if (!dialog.isCanceled() && !dialog.getResultPath().empty())
    {
        filePath  = dialog.getResultPath();
        assetName = std::filesystem::path(filePath).filename().string();
    }
}

void LoadStepAssetWindow::executeLoad()
{
    if (!gatedDatabase || !defer || filePath.empty())
    {
        return;
    }

    GatedDatabase* db   = gatedDatabase;
    std::string    path = filePath;
    std::string    name = assetName.empty() ? std::filesystem::path(filePath).filename().string() : assetName;

    defer->defer(
      [db, path, name]()
      {
          db->executeAsync({
            {     "type", "LoadStepAsset" },
            {  "version",               1 },
            {     "name",            name },
            { "filePath",            path }
          });
      });
}

void LoadStepAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Load STEP Asset##LoadStepWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
    {
        char pathBuffer[512];
        strncpy_s(pathBuffer, sizeof(pathBuffer), filePath.c_str(), _TRUNCATE);
        if (ImGui::InputText("File Path", pathBuffer, sizeof(pathBuffer)))
        {
            filePath = std::string(pathBuffer);
            if (assetName.empty() && !filePath.empty())
            {
                assetName = std::filesystem::path(filePath).filename().string();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            browseFile();
        }

        char nameBuffer[256];
        strncpy_s(nameBuffer, sizeof(nameBuffer), assetName.c_str(), _TRUNCATE);
        if (ImGui::InputText("Asset Name", nameBuffer, sizeof(nameBuffer)))
        {
            assetName = std::string(nameBuffer);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::BeginDisabled(filePath.empty());
        if (ImGui::Button("Load", ImVec2(120.0f, 0.0f)))
        {
            executeLoad();
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
