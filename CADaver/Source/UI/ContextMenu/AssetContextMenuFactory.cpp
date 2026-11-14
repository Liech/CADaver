#include "UI/ContextMenu/AssetContextMenuFactory.h"

#include "Execution/GatedDatabase.h"
#include "UI/ContextMenu/CadAssetContextMenu.h"
#include "UI/ContextMenu/TriangulationAssetContextMenu.h"
#include "UI/ContextMenu/VoxelAssetContextMenu.h"
#include "Windows/Project/RemoveAssetModal.h"
#include "Windows/Windows.h"

#include <imgui.h>

AssetContextMenuFactory::AssetContextMenuFactory() {}

AssetContextMenuFactory::~AssetContextMenuFactory() {}

std::unique_ptr<AssetContextMenu> AssetContextMenuFactory::create(const std::string& assetType)
{
    if (assetType == "Cad")
    {
        return std::make_unique<CadAssetContextMenu>();
    }
    else if (assetType == "Triangulation")
    {
        return std::make_unique<TriangulationAssetContextMenu>();
    }
    else if (assetType == "Voxel")
    {
        return std::make_unique<VoxelAssetContextMenu>();
    }

    return nullptr;
}

void AssetContextMenuFactory::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType)
{
    std::string popupId = "AssetContextMenu##" + assetGuid.toString();
    if (ImGui::BeginPopupContextItem(popupId.c_str()))
    {
        auto menu = create(assetType);
        if (menu)
        {
            menu->render(gatedDatabase, windows, assetGuid);
        }

        if (ImGui::MenuItem("Add Note..."))
        {
            Polyglot::Node command = {
                {      "type",            "AddNote" },
                {   "version",                    1 },
                { "assetGuid", assetGuid.toString() },
                {      "name",               "Note" },
                {      "text",                   "" }
            };
            gatedDatabase.execute(command);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Remove Asset"))
        {
            windows.open("RemoveAsset");
            auto modal = std::dynamic_pointer_cast<RemoveAssetModal>(windows.get("RemoveAsset"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }

        ImGui::EndPopup();
    }
}
