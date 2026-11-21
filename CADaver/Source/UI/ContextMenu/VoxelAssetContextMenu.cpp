#include "UI/ContextMenu/VoxelAssetContextMenu.h"

#include "Execution/GatedDatabase.h"
#include "Windows/Asset/RigidBodyEditWindow.h"
#include "Windows/Conversion/SvoToStlConversionModal.h"
#include "Windows/Conversion/SvoToStlMarchingCubesConversionModal.h"
#include "Windows/Conversion/SvoToStlSurfaceNetsConversionModal.h"
#include "Windows/IO/SaveAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

VoxelAssetContextMenu::VoxelAssetContextMenu() {}

VoxelAssetContextMenu::~VoxelAssetContextMenu() {}

void VoxelAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
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
        if (ImGui::MenuItem("STL (Blocky)..."))
        {
            windows.open("SvoToStlConversion");
            auto modal = std::dynamic_pointer_cast<SvoToStlConversionModal>(windows.get("SvoToStlConversion"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }
        if (ImGui::MenuItem("STL (SurfaceNets)..."))
        {
            windows.open("SvoToStlSurfaceNetsConversion");
            auto modal = std::dynamic_pointer_cast<SvoToStlSurfaceNetsConversionModal>(windows.get("SvoToStlSurfaceNetsConversion"));
            if (modal)
            {
                modal->setTargetAsset(assetGuid);
            }
        }
        if (ImGui::MenuItem("STL (Marching Cubes)..."))
        {
            windows.open("SvoToStlMarchingCubesConversion");
            auto modal = std::dynamic_pointer_cast<SvoToStlMarchingCubesConversionModal>(windows.get("SvoToStlMarchingCubesConversion"));
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
