#include "TriangulationAssetContextMenu.h"

#include "Database/Asset/TriangulationAsset.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Dialog/SaveFileDialog.h"
#include "Windows/Conversion/StlToSvoConversionModal.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(GatedDatabase& gatedDatabase, const Asset& asset)
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
            if (stlToSvoModal)
            {
                stlToSvoModal->setTargetAsset(triAsset->getGuid());
                stlToSvoModal->open();
            }
        }
        ImGui::EndMenu();
    }
}

void TriangulationAssetContextMenu::setStlToSvoModal(StlToSvoConversionModal* modal)
{
    stlToSvoModal = modal;
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
