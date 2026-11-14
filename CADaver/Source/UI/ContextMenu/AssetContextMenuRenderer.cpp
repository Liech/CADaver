#include "UI/ContextMenu/AssetContextMenuRenderer.h"

#include "Execution/GatedDatabase.h"
#include "UI/ContextMenu/AssetContextMenu.h"
#include "UI/ContextMenu/AssetContextMenuFactory.h"
#include "Windows/Project/RemoveAssetModal.h"
#include "Windows/Windows.h"

#include <imgui.h>

AssetContextMenuRenderer::AssetContextMenuRenderer()
  : factory(std::make_unique<AssetContextMenuFactory>())
{
}

AssetContextMenuRenderer::~AssetContextMenuRenderer() {}

void AssetContextMenuRenderer::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType)
{
    std::string popupId = "AssetContextMenu##" + assetGuid.toString();
    if (ImGui::BeginPopupContextItem(popupId.c_str()))
    {
        renderMenuContents(gatedDatabase, windows, assetGuid, assetType);
        ImGui::EndPopup();
    }
}

void AssetContextMenuRenderer::renderPopup(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType, const std::string& popupId)
{
    if (ImGui::BeginPopup(popupId.c_str()))
    {
        renderMenuContents(gatedDatabase, windows, assetGuid, assetType);
        ImGui::EndPopup();
    }
}

void AssetContextMenuRenderer::renderMenuContents(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid, const std::string& assetType)
{
    auto menu = factory->create(assetType);
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
}
