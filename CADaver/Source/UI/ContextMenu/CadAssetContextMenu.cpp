#include "UI/ContextMenu/CadAssetContextMenu.h"

#include "Database/Asset/CadAsset.h"
#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/IO/SaveStepAssetWindow.h"
#include "Windows/WindowContainer.h"

#include <imgui.h>
#include <memory>

CadAssetContextMenu::CadAssetContextMenu() {}

CadAssetContextMenu::~CadAssetContextMenu() {}

void CadAssetContextMenu::render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset)
{
    (void)gatedDatabase;
    auto cadAsset = dynamic_cast<const CadAsset*>(&asset);
    if (!cadAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save..."))
    {
        windowContainer.open("SaveStepAssetWindow");
        auto window = std::dynamic_pointer_cast<SaveStepAssetWindow>(windowContainer.get("SaveStepAssetWindow"));
        if (window)
        {
            window->setTargetAsset(cadAsset->getGuid());
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL..."))
        {
            windowContainer.open("CadToStlConversionModal");
            auto modal = std::dynamic_pointer_cast<CadToStlConversionModal>(windowContainer.get("CadToStlConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(cadAsset->getGuid());
            }
        }
        if (ImGui::MenuItem("SVO..."))
        {
            windowContainer.open("CadToSvoConversionModal");
            auto modal = std::dynamic_pointer_cast<CadToSvoConversionModal>(windowContainer.get("CadToSvoConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(cadAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}
