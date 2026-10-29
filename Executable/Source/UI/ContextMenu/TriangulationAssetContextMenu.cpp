#include "TriangulationAssetContextMenu.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Library/Util/SaveFileDialog.h"
#include "UI/Converter/StlToSvoConversionModal.h"

#include <filesystem>
#include <fstream>
#include <imgui.h>

TriangulationAssetContextMenu::TriangulationAssetContextMenu() {}

TriangulationAssetContextMenu::~TriangulationAssetContextMenu() {}

void TriangulationAssetContextMenu::render(Asset& asset)
{
    auto triAsset = dynamic_cast<TriangulationAsset*>(&asset);
    if (!triAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save"))
    {
        save(*triAsset);
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("SVO..."))
        {
            if (stlToSvoModal)
            {
                stlToSvoModal->open(getAssetGuid(), triAsset->getName());
            }
        }
        ImGui::EndMenu();
    }
}

void TriangulationAssetContextMenu::setStlToSvoModal(StlToSvoConversionModal* modal)
{
    stlToSvoModal = modal;
}

void TriangulationAssetContextMenu::save(TriangulationAsset& asset)
{
    if (!asset.getTriangulation())
    {
        return;
    }

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

        CADacomb::GUID targetGuid = getAssetGuid();
        if (targetGuid.toString().empty())
        {
            targetGuid = GatedDatabase::getInstance().get().getProject().getSelectedAssetGuid();
        }

        if (!targetGuid.toString().empty())
        {
            Defer::getInstance().defer(
              [targetGuid, savePath]()
              {
                  GatedDatabase::getInstance().executeAsync({
                    {      "type",        "SaveStlAsset" },
                    {   "version",                     1 },
                    { "assetGuid", targetGuid.toString() },
                    {  "filePath",              savePath }
                  });
              });
        }
    }
}
