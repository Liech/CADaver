#include "UI/ContextMenu/CadAssetContextMenu.h"

#include "Database/Asset/CadAsset.h"
#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/IO/SaveStepAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

CadAssetContextMenu::CadAssetContextMenu() {}

CadAssetContextMenu::~CadAssetContextMenu() {}

void CadAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const Asset& asset)
{
    (void)gatedDatabase;
    auto cadAsset = dynamic_cast<const CadAsset*>(&asset);
    if (!cadAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save..."))
    {
        windows.open("SaveStepAssetWindow");
        auto window = std::dynamic_pointer_cast<SaveStepAssetWindow>(windows.get("SaveStepAssetWindow"));
        if (window)
        {
            window->setTargetAsset(cadAsset->getGuid());
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL..."))
        {
            windows.open("CadToStlConversionModal");
            auto modal = std::dynamic_pointer_cast<CadToStlConversionModal>(windows.get("CadToStlConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(cadAsset->getGuid());
            }
        }
        if (ImGui::MenuItem("SVO..."))
        {
            windows.open("CadToSvoConversionModal");
            auto modal = std::dynamic_pointer_cast<CadToSvoConversionModal>(windows.get("CadToSvoConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(cadAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}
