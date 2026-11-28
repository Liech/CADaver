#include "UI/ContextMenu/SdfAssetContextMenu.h"

#include "Execution/GatedDatabase.h"
#include "Windows/Asset/SdfEditWindow.h"
#include "Windows/Conversion/SdfToStlConversionModal.h"
#include "Windows/Conversion/SdfToSvoConversionModal.h"
#include "Windows/IO/SaveAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

SdfAssetContextMenu::SdfAssetContextMenu() {}

SdfAssetContextMenu::~SdfAssetContextMenu() {}

void SdfAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
{
    (void)gatedDatabase;

    if (ImGui::MenuItem("Edit SDF..."))
    {
        windows.open("SdfEdit");
        auto window = std::dynamic_pointer_cast<SdfEditWindow>(windows.get("SdfEdit"));
        if (window)
        {
            window->setTargetAsset(assetGuid);
        }
    }

    if (ImGui::MenuItem("Save..."))
    {
        windows.open("SaveAsset");
        auto window = std::dynamic_pointer_cast<SaveAssetWindow>(windows.get("SaveAsset"));
        if (window)
        {
            window->setTargetAsset(assetGuid);
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("SVO..."))
        {
            windows.open("SdfToSvoConversion");
            auto modal = std::dynamic_pointer_cast<SdfToSvoConversionModal>(windows.get("SdfToSvoConversion"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }
        if (ImGui::MenuItem("STL (SurfaceNets)..."))
        {
            windows.open("SdfToStlConversion");
            auto modal = std::dynamic_pointer_cast<SdfToStlConversionModal>(windows.get("SdfToStlConversion"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }
        ImGui::EndMenu();
    }
}
