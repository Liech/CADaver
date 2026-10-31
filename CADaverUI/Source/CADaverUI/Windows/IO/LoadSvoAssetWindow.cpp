#include "Windows/IO/LoadSvoAssetWindow.h"

#include "Dialog/LoadFileDialog.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <memory>

LoadSvoAssetWindow::LoadSvoAssetWindow() {}

LoadSvoAssetWindow::~LoadSvoAssetWindow() {}

std::string LoadSvoAssetWindow::getType() const
{
    return "LoadSvoAssetWindow";
}

std::string LoadSvoAssetWindow::getDescription() const
{
    return "Window: LoadSvoAssetWindow\n"
           "Title: Load SVO Asset\n"
           "Purpose: Dialog for selecting a Sparse Voxel Octree (.cadacombsvo / .svo) file on disk and importing it into the project as a VoxelAsset.\n\n"
           "Parameters:\n"
           "  - File Path: Path to the source .cadacombsvo or .svo file on disk\n"
           "  - Asset Name: Display name for the loaded voxel asset in the project\n\n"
           "Dispatched Command: LoadSvoAsset (async)";
}

void LoadSvoAssetWindow::browseFile()
{
    LoadFileDialog dialog;
    dialog.addFilter("SVO File", { "cadacombsvo", "svo" });
    dialog.execute();

    if (!dialog.isCanceled() && !dialog.getResultPath().empty())
    {
        filePath  = dialog.getResultPath();
        assetName = std::filesystem::path(filePath).filename().string();
    }
}

void LoadSvoAssetWindow::executeLoad()
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
          auto fileStream = std::make_shared<std::ifstream>(path, std::ios::binary);
          db->executeAsync(
            {
              {    "type", "LoadSvoAsset" },
              { "version",              1 },
              {    "name",           name }
          },
            fileStream);
      });
}

void LoadSvoAssetWindow::render()
{
    if (!openState || !gatedDatabase)
    {
        return;
    }

    if (ImGui::Begin("Load SVO Asset##LoadSvoWindow", &openState, ImGuiWindowFlags_AlwaysAutoResize))
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
