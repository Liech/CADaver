#include "UI/ContextMenu/VectorGraphicsAssetContextMenu.h"

#include "Windows/IO/SaveAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

VectorGraphicsAssetContextMenu::VectorGraphicsAssetContextMenu() {}

VectorGraphicsAssetContextMenu::~VectorGraphicsAssetContextMenu() {}

void VectorGraphicsAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
{
    if (ImGui::MenuItem("Save..."))
    {
        windows.open("SaveAsset");
        auto window = std::dynamic_pointer_cast<SaveAssetWindow>(windows.get("SaveAsset"));
        if (window)
        {
            window->setTargetAsset(assetGuid);
        }
    }
}
