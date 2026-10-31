#include "VoxelAssetContextMenu.h"

#include "CADacomb/Volume/SVO/SVOFile.h"
#include "Database/Asset/VoxelAsset.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Dialog/SaveFileDialog.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

VoxelAssetContextMenu::VoxelAssetContextMenu() {}

VoxelAssetContextMenu::~VoxelAssetContextMenu() {}

void VoxelAssetContextMenu::render(GatedDatabase& gatedDatabase, const Asset& asset)
{
    auto voxelAsset = dynamic_cast<const VoxelAsset*>(&asset);
    if (!voxelAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save"))
    {
        save(gatedDatabase, *voxelAsset);
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL (Blocky)..."))
        {
            if (svoToStlModal)
            {
                svoToStlModal->setTargetAsset(voxelAsset->getGuid());
                svoToStlModal->open();
            }
        }
        ImGui::EndMenu();
    }
}

void VoxelAssetContextMenu::setSvoToStlModal(SvoToStlConversionModal* modal)
{
    svoToStlModal = modal;
}

void VoxelAssetContextMenu::save(GatedDatabase& gatedDatabase, const VoxelAsset& asset)
{
    Library::SaveFileDialog dialog;
    dialog.addFilter("SVO File", { "cadacombsvo", "svo" });

    std::string defaultName = asset.getName();
    if (std::filesystem::path(defaultName).extension().empty())
    {
        defaultName += ".cadacombsvo";
    }
    dialog.setSaveFileName(defaultName);

    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string savePath = dialog.getResultPath();
        if (std::filesystem::path(savePath).extension().empty())
        {
            savePath += ".cadacombsvo";
        }

        CADacomb::GUID targetGuid = asset.getGuid();
        Defer::getInstance().defer(
          [&gatedDatabase, targetGuid, savePath]()
          {
              auto fileStream = std::make_shared<std::ofstream>(savePath, std::ios::binary);
              gatedDatabase.executeAsync(
                {
                  {      "type",      "SaveVoxelAsset" },
                  {   "version",                     1 },
                  { "assetGuid", targetGuid.toString() }
              },
                nullptr,
                fileStream);
          });
    }
}
