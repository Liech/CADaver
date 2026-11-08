#include "UI/ContextMenu/VoxelAssetContextMenu.h"

#include "Database/Asset/VoxelAsset.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"
#include "Windows/IO/SaveVoxelAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

VoxelAssetContextMenu::VoxelAssetContextMenu() {}

VoxelAssetContextMenu::~VoxelAssetContextMenu() {}

void VoxelAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const Asset& asset)
{
    (void)gatedDatabase;
    auto voxelAsset = dynamic_cast<const VoxelAsset*>(&asset);
    if (!voxelAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save..."))
    {
        windows.open("SaveVoxelAsset");
        auto window = std::dynamic_pointer_cast<SaveVoxelAssetWindow>(windows.get("SaveVoxelAsset"));
        if (window)
        {
            window->setTargetAsset(voxelAsset->getGuid());
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL (Blocky)..."))
        {
            windows.open("SvoToStlConversion");
            auto modal = std::dynamic_pointer_cast<SvoToStlConversionModal>(windows.get("SvoToStlConversion"));
            if (modal)
            {
                modal->setTargetAsset(voxelAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}
