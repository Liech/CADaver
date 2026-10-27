#include "VoxelAssetContextMenu.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/VoxelAsset.h"
#include "Library/Util/SaveFileDialog.h"
#include "UI/Converter/SvoToStlConversionModal.h"

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
        if (ImGui::MenuItem("STL (Blocky)..."))
        {
            if (svoToStlModal)
            {
                int maxDepth = 6;
                if (voxelAsset->getSVO())
                {
                    uint32_t dim = voxelAsset->getSVO()->getDimension();
                    uint32_t d   = 0;
                    while ((1u << d) < dim)
                    {
                        d++;
                    }
                    if (d > 0)
                    {
                        maxDepth = static_cast<int>(d);
                    }
                }
                svoToStlModal->open(getAssetIndex(), voxelAsset->getName(), maxDepth);
            }
        }
        ImGui::EndMenu();
    }
}

void VoxelAssetContextMenu::setSvoToStlModal(SvoToStlConversionModal* modal)
{
    svoToStlModal = modal;
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

        if (targetIndex >= 0)
        {
            Defer::getInstance().defer(
              [targetIndex, savePath]()
              {
                  GatedDatabase::getInstance().execute({
                    {       "type", "SaveVoxelAsset" },
                    {    "version",                1 },
                    { "assetIndex",      targetIndex },
                    {   "filePath",         savePath }
                  });
              });
        }
    }
}
