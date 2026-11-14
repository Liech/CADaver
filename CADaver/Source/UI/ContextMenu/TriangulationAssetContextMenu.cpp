#include "UI/ContextMenu/TriangulationAssetContextMenu.h"

#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/IO/SaveStlAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
{
    (void)gatedDatabase;

    if (ImGui::MenuItem("Save..."))
    {
        windows.open("SaveStlAsset");
        auto window = std::dynamic_pointer_cast<SaveStlAssetWindow>(windows.get("SaveStlAsset"));
        if (window)
        {
            window->setTargetAsset(assetGuid);
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
                modal->setTargetAsset(assetGuid);
            }
        }
        ImGui::EndMenu();
    }
}
