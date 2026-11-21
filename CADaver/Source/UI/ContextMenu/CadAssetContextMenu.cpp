#include "UI/ContextMenu/CadAssetContextMenu.h"

#include "Execution/GatedDatabase.h"
#include "Windows/Asset/RigidBodyEditWindow.h"
#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/IO/SaveAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

CadAssetContextMenu::CadAssetContextMenu() {}

CadAssetContextMenu::~CadAssetContextMenu() {}

void CadAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
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

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL..."))
        {
            windows.open("CadToStlConversion");
            auto modal = std::dynamic_pointer_cast<CadToStlConversionModal>(windows.get("CadToStlConversion"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }
        if (ImGui::MenuItem("SVO..."))
        {
            windows.open("CadToSvoConversion");
            auto modal = std::dynamic_pointer_cast<CadToSvoConversionModal>(windows.get("CadToSvoConversion"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }
        ImGui::EndMenu();
    }

    if (ImGui::MenuItem("Add RigidBody..."))
    {
        Polyglot::Node command = {
            {          "type",                              "AddComponent" },
            {       "version",                                           1 },
            {     "assetGuid",                        assetGuid.toString() },
            { "componentType",                                 "RigidBody" },
            {          "name",                                 "RigidBody" },
            {          "data", { { "Version", 1 }, { "isStatic", false } } }
        };
        Polyglot::Node result = gatedDatabase.execute(command);
        if (result["type"].asString() == "AddComponent")
        {
            windows.open("RigidBodyEdit");
            auto editWindow = std::dynamic_pointer_cast<RigidBodyEditWindow>(windows.get("RigidBodyEdit"));
            if (editWindow)
            {
                editWindow->setTarget(assetGuid, CADacomb::GUID(result["componentGuid"].asString()));
            }
        }
    }
}
