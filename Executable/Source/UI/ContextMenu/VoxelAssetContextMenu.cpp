#include "VoxelAssetContextMenu.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Commands/GatedDatabase.h"
#include "Library/Util/SaveFileDialog.h"
#include "Project/VoxelAsset.h"

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

        std::ofstream fileStream(savePath, std::ios::binary);
        int           selectedIndex = GatedDatabase::getInstance().get().getProject().getSelectedIndex();
        GatedDatabase::getInstance().execute(
          {
            {       "type", "SaveVoxelAsset" },
            {    "version",                1 },
            { "assetIndex",    selectedIndex }
        },
          nullptr,
          &fileStream);

        asset.setFilePath(savePath);
    }
}
