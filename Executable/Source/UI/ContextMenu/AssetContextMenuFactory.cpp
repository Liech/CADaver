#include "AssetContextMenuFactory.h"

#include "AssetContextMenu.h"
#include "CadAssetContextMenu.h"
#include "Project/Asset.h"
#include "Project/AssetType.h"
#include "TriangulationAssetContextMenu.h"

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

    return nullptr;
}

void AssetContextMenuFactory::render(const std::shared_ptr<Asset>& asset)
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
            menu->render(*asset);
        }
        ImGui::EndPopup();
    }
}
