#include "UI/ContextMenu/VoxelAssetContextMenu.h"

#include "Database/Asset/VoxelAsset.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"
#include "Windows/IO/SaveVoxelAssetWindow.h"
#include "Windows/WindowContainer.h"

#include <imgui.h>
#include <memory>

VoxelAssetContextMenu::VoxelAssetContextMenu() {}

VoxelAssetContextMenu::~VoxelAssetContextMenu() {}

void VoxelAssetContextMenu::render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset)
{
    (void)gatedDatabase;
    auto voxelAsset = dynamic_cast<const VoxelAsset*>(&asset);
    if (!voxelAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save..."))
    {
        windowContainer.open("SaveVoxelAssetWindow");
        auto window = std::dynamic_pointer_cast<SaveVoxelAssetWindow>(windowContainer.get("SaveVoxelAssetWindow"));
        if (window)
        {
            window->setTargetAsset(voxelAsset->getGuid());
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL (Blocky)..."))
        {
            windowContainer.open("SvoToStlConversionModal");
            auto modal = std::dynamic_pointer_cast<SvoToStlConversionModal>(windowContainer.get("SvoToStlConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(voxelAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}
