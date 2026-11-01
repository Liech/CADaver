#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "UI/ContextMenu/CadAssetContextMenu.h"
#include "UI/ContextMenu/TriangulationAssetContextMenu.h"
#include "UI/ContextMenu/VoxelAssetContextMenu.h"
#include "Windows/Project/RemoveAssetModal.h"
#include "Windows/Windows.h"

#include <imgui.h>

AssetContextMenuFactory::AssetContextMenuFactory() {}

AssetContextMenuFactory::~AssetContextMenuFactory() {}

std::unique_ptr<AssetContextMenu> AssetContextMenuFactory::create(const std::shared_ptr<const Asset>& asset)
{
    if (!asset)
    {
        return nullptr;
    }

    if (asset->getType() == "Cad")
    {
        return std::make_unique<CadAssetContextMenu>();
    }
    else if (asset->getType() == "Triangulation")
    {
        return std::make_unique<TriangulationAssetContextMenu>();
    }
    else if (asset->getType() == "Voxel")
    {
        return std::make_unique<VoxelAssetContextMenu>();
    }

    return nullptr;
}

void AssetContextMenuFactory::render(GatedDatabase& gatedDatabase, Windows& windows, const std::shared_ptr<const Asset>& asset)
{
    if (!asset)
    {
        return;
    }

    std::string popupId = "AssetContextMenu##" + std::to_string(reinterpret_cast<uintptr_t>(asset.get()));
    if (ImGui::BeginPopupContextItem(popupId.c_str()))
    {
        auto menu = create(asset);
        if (menu)
        {
            menu->render(gatedDatabase, windows, *asset);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Close"))
        {
            windows.open("RemoveAsset");
            auto modal = std::dynamic_pointer_cast<RemoveAssetModal>(windows.get("RemoveAsset"));
            if (modal)
            {
                modal->setTargetAsset(asset->getGuid());
            }
        }

        ImGui::EndPopup();
    }
}
