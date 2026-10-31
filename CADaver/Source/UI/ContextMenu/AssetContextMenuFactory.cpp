#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include "Database/Asset/CadAsset.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Database/Asset/VoxelAsset.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/ContextMenu/CadAssetContextMenu.h"
#include "UI/ContextMenu/TriangulationAssetContextMenu.h"
#include "UI/ContextMenu/VoxelAssetContextMenu.h"

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

void AssetContextMenuFactory::render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const std::shared_ptr<const Asset>& asset)
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
            menu->render(gatedDatabase, windowContainer, *asset);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Close"))
        {
            closeTargetGuid = asset->getGuid();
            closeTargetName = asset->getName();
            openCloseModal  = true;
        }

        ImGui::EndPopup();
    }
}

void AssetContextMenuFactory::renderCloseModal(GatedDatabase& gatedDatabase)
{
    if (openCloseModal)
    {
        ImGui::OpenPopup("Close Asset##Modal");
        openCloseModal = false;
    }

    if (ImGui::BeginPopupModal("Close Asset##Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Are you sure you want to close '%s'?", closeTargetName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Yes", ImVec2(120.0f, 0.0f)))
        {
            CADacomb::GUID targetGuid = closeTargetGuid;
            Defer::getInstance().defer(
              [&gatedDatabase, targetGuid]()
              {
                  gatedDatabase.execute({
                    {      "type",         "RemoveAsset" },
                    {   "version",                     1 },
                    { "assetGuid", targetGuid.toString() }
                  });
              });
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
