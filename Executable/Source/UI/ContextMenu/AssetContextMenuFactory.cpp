#include "AssetContextMenuFactory.h"

#include "AssetContextMenu.h"
#include "CadAssetContextMenu.h"
#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset.h"
#include "Database/AssetType.h"
#include "TriangulationAssetContextMenu.h"
#include "VoxelAssetContextMenu.h"

#include <cstdint>
#include <imgui.h>
#include <string>

AssetContextMenuFactory::AssetContextMenuFactory() {}

AssetContextMenuFactory::~AssetContextMenuFactory() {}

std::unique_ptr<AssetContextMenu> AssetContextMenuFactory::create(const std::shared_ptr<Asset>& asset)
{
    if (!asset)
    {
        return nullptr;
    }

    if (asset->getType() == AssetType::Triangulation)
    {
        return std::make_unique<TriangulationAssetContextMenu>();
    }
    else if (asset->getType() == AssetType::Cad)
    {
        return std::make_unique<CadAssetContextMenu>();
    }
    else if (asset->getType() == AssetType::Voxel)
    {
        return std::make_unique<VoxelAssetContextMenu>();
    }

    return nullptr;
}

void AssetContextMenuFactory::render(const std::shared_ptr<Asset>& asset)
{
    render(asset, -1);
}

void AssetContextMenuFactory::render(const std::shared_ptr<Asset>& asset, int assetIndex)
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
            menu->setAssetIndex(assetIndex);
            menu->render(*asset);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Close"))
        {
            closeTargetIndex = assetIndex;
            closeTargetName  = asset->getName();
            openCloseModal   = true;
        }

        ImGui::EndPopup();
    }
}

void AssetContextMenuFactory::renderCloseModal()
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
            int targetIndex = closeTargetIndex;
            Defer::getInstance().defer(
              [targetIndex]()
              {
                  GatedDatabase::getInstance().execute({
                    {       "type", "RemoveAsset" },
                    {    "version",             1 },
                    { "assetIndex",   targetIndex }
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
