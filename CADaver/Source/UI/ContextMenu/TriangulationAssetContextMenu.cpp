#include "UI/ContextMenu/TriangulationAssetContextMenu.h"

#include "Windows/Geometry/CalculateConnectedComponentsWindow.h"
#include "Windows/Geometry/ClusterByNormalWindow.h"
#include "Windows/Geometry/SimplifyMeshWindow.h"

#include "Execution/GatedDatabase.h"
#include "Windows/Conversion/ConvertClusteringToCadWindow.h"
#include "Windows/Conversion/ConvertStlToCadWindow.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/IO/SaveAssetWindow.h"
#include "Windows/Windows.h"

#include <imgui.h>
#include <memory>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(GatedDatabase& gatedDatabase, Windows& windows, const CADacomb::GUID& assetGuid)
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

    if (ImGui::MenuItem("Simplify Mesh..."))
    {
        windows.open("SimplifyMesh");
        auto window = std::dynamic_pointer_cast<SimplifyMeshWindow>(windows.get("SimplifyMesh"));
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
        if (ImGui::MenuItem("CAD (from Clustering)..."))
        {
            windows.open("ConvertClusteringToCad");
            auto window = std::dynamic_pointer_cast<ConvertClusteringToCadWindow>(windows.get("ConvertClusteringToCad"));
            if (window)
            {
                window->setTargetAsset(assetGuid);
            }
        }
        if (ImGui::MenuItem("CAD (Naive)..."))
        {
            windows.open("ConvertStlToCad");
            auto window = std::dynamic_pointer_cast<ConvertStlToCadWindow>(windows.get("ConvertStlToCad"));
            if (window)
            {
                window->setTargetAsset(assetGuid);
            }
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Clustering"))
    {
        if (ImGui::MenuItem("Calculate Connected Components..."))
        {
            windows.open("CalculateConnectedComponents");
            auto window = std::dynamic_pointer_cast<CalculateConnectedComponentsWindow>(windows.get("CalculateConnectedComponents"));
            if (window)
            {
                window->setTargetAsset(assetGuid);
            }
        }

        if (ImGui::MenuItem("Cluster by Normal..."))
        {
            windows.open("ClusterByNormal");
            auto window = std::dynamic_pointer_cast<ClusterByNormalWindow>(windows.get("ClusterByNormal"));
            if (window)
            {
                window->setTargetAsset(assetGuid);
            }
        }
        ImGui::EndMenu();
    }
}
