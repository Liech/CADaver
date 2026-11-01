#include "UI/ContextMenu/TriangulationAssetContextMenu.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/IO/SaveStlAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const Asset& asset)
{
    (void)gatedDatabase;
    auto triAsset = dynamic_cast<const TriangulationAsset*>(&asset);
    if (!triAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save..."))
    {
        windows.open("SaveStlAsset");
        auto window = std::dynamic_pointer_cast<SaveStlAssetWindow>(windows.get("SaveStlAsset"));
        if (window)
        {
            window->setTargetAsset(triAsset->getGuid());
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("SVO..."))
        {
            windows.open("StlToSvoConversion");
            auto modal = std::dynamic_pointer_cast<StlToSvoConversionModal>(windows.get("StlToSvoConversion"));
            if (modal)
            {
                modal->setTargetAsset(triAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}
