#include "VoxelAssetContextMenu.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/VoxelAsset.h"
#include "Library/Util/SaveFileDialog.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

VoxelAssetContextMenu::VoxelAssetContextMenu() {}

VoxelAssetContextMenu::~VoxelAssetContextMenu() {}

void VoxelAssetContextMenu::render(Asset& asset)
{
    auto voxelAsset = dynamic_cast<VoxelAsset*>(&asset);
    if (!voxelAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save"))
    {
        save(*voxelAsset);
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL (Blocky)"))
        {
            int targetIndex = getAssetIndex();
            Defer::getInstance().defer(
              [targetIndex]()
              {
                  GatedDatabase::getInstance().execute({
                    {       "type", "ConvertSvoToStl" },
                    {    "version",                 1 },
                    { "assetIndex",       targetIndex }
                  });
              });
        }
        ImGui::EndMenu();
    }
}

void VoxelAssetContextMenu::save(VoxelAsset& asset)
{
    if (!asset.getSVO())
    {
        return;
    }

    Library::SaveFileDialog dialog;
    dialog.addFilter("SVO File", { "cadacombsvo", "svo" });

    if (!asset.getFilePath().empty())
    {
        std::filesystem::path existingPath(asset.getFilePath());
        dialog.setDefaultPath(existingPath.parent_path().string());
        dialog.setSaveFileName(existingPath.filename().string());
    }
    else
    {
        std::string defaultName = asset.getName();
        if (std::filesystem::path(defaultName).extension().empty())
        {
            defaultName += ".cadacombsvo";
        }
        dialog.setSaveFileName(defaultName);
    }

    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string savePath = dialog.getResultPath();
        if (std::filesystem::path(savePath).extension().empty())
        {
            savePath += ".cadacombsvo";
        }

        int targetIndex = getAssetIndex();
        if (targetIndex < 0)
        {
            targetIndex = GatedDatabase::getInstance().get().getProject().getSelectedIndex();
        }

        Defer::getInstance().defer(
          [targetIndex, savePath]()
          {
              std::ofstream fileStream(savePath, std::ios::binary);
              GatedDatabase::getInstance().execute(
                {
                  {       "type", "SaveVoxelAsset" },
                  {    "version",                1 },
                  { "assetIndex",      targetIndex }
              },
                nullptr,
                &fileStream);
          });

        asset.setFilePath(savePath);
    }
}
