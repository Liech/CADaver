#include "UI/ContextMenu/TriangulationAssetContextMenu.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/IO/SaveStlAssetWindow.h"
#include "Windows/WindowContainer.h"

#include <imgui.h>
#include <memory>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset)
{
    (void)gatedDatabase;
    auto triAsset = dynamic_cast<const TriangulationAsset*>(&asset);
    if (!triAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save..."))
    {
        windowContainer.open("SaveStlAssetWindow");
        auto window = std::dynamic_pointer_cast<SaveStlAssetWindow>(windowContainer.get("SaveStlAssetWindow"));
        if (window)
        {
            window->setTargetAsset(triAsset->getGuid());
        }
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("SVO..."))
        {
            windowContainer.open("StlToSvoConversionModal");
            auto modal = std::dynamic_pointer_cast<StlToSvoConversionModal>(windowContainer.get("StlToSvoConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(triAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}
