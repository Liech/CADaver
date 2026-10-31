#include "UI/ContextMenu/TriangulationAssetContextMenu.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Dialog/SaveFileDialog.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"
#include "Windows/WindowContainer.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <memory>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset)
{
    auto triAsset = dynamic_cast<const TriangulationAsset*>(&asset);
    if (!triAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save"))
    {
        save(gatedDatabase, *triAsset);
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

void TriangulationAssetContextMenu::save(GatedDatabase& gatedDatabase, const TriangulationAsset& asset)
{
    Library::SaveFileDialog dialog;
    dialog.addFilter("STL File", { "stl" });

    std::string defaultName = asset.getName();
    if (std::filesystem::path(defaultName).extension().empty())
    {
        defaultName += ".stl";
    }
    dialog.setSaveFileName(defaultName);
    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string savePath = dialog.getResultPath();
        if (std::filesystem::path(savePath).extension().empty())
        {
            savePath += ".stl";
        }

        CADacomb::GUID targetGuid = asset.getGuid();
        Defer::getInstance().defer(
          [&gatedDatabase, targetGuid, savePath]()
          {
              auto fileStream = std::make_shared<std::ofstream>(savePath, std::ios::binary);
              gatedDatabase.executeAsync(
                {
                  {      "type",        "SaveStlAsset" },
                  {   "version",                     1 },
                  { "assetGuid", targetGuid.toString() }
              },
                nullptr,
                fileStream);
          });
    }
}
