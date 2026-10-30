#include "VoxelAssetContextMenu.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/VoxelAsset.h"
#include "CADaverLibrary/Util/SaveFileDialog.h"
#include "UI/Converter/SvoToStlConversionModal.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

VoxelAssetContextMenu::VoxelAssetContextMenu() {}

VoxelAssetContextMenu::~VoxelAssetContextMenu() {}

void VoxelAssetContextMenu::render(const Asset& asset)
{
    auto voxelAsset = dynamic_cast<const VoxelAsset*>(&asset);
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
                svoToStlModal->open(getAssetGuid(), voxelAsset->getName(), maxDepth);
            }
        }
        ImGui::EndMenu();
    }
}

void VoxelAssetContextMenu::setSvoToStlModal(SvoToStlConversionModal* modal)
{
    svoToStlModal = modal;
}

void VoxelAssetContextMenu::save(const VoxelAsset& asset)
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

        CADacomb::GUID targetGuid = getAssetGuid();
        if (targetGuid.toString().empty())
        {
            targetGuid = GatedDatabase::getInstance().get().getProject().getSelectedAssetGuid();
        }

        if (!targetGuid.toString().empty())
        {
            Defer::getInstance().defer(
              [targetGuid, savePath]()
              {
                  auto fileStream = std::make_shared<std::ofstream>(savePath, std::ios::binary);
                  GatedDatabase::getInstance().executeAsync(
                    {
                      {      "type",      "SaveVoxelAsset" },
                      {   "version",                     1 },
                      { "assetGuid", targetGuid.toString() }
                  },
                    false,
                    nullptr,
                    fileStream);
              });
        }
    }
}
