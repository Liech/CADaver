#include "CadAssetContextMenu.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/CadAsset.h"
#include "UI/Converter/CadToStlConversionModal.h"
#include "UI/Converter/CadToSvoConversionModal.h"
#include "UI/Dialog/SaveFileDialog.h"

#include <filesystem>
#include <imgui.h>

CadAssetContextMenu::CadAssetContextMenu() {}

CadAssetContextMenu::~CadAssetContextMenu() {}

void CadAssetContextMenu::render(const Asset& asset)
{
    auto cadAsset = dynamic_cast<const CadAsset*>(&asset);
    if (!cadAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save"))
    {
        save(*cadAsset);
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL..."))
        {
            if (cadToStlModal)
            {
                cadToStlModal->open(cadAsset->getGuid(), cadAsset->getName());
            }
        }
        if (ImGui::MenuItem("SVO..."))
        {
            if (cadToSvoModal)
            {
                cadToSvoModal->open(cadAsset->getGuid(), cadAsset->getName());
            }
        }
        ImGui::EndMenu();
    }
}

void CadAssetContextMenu::setCadToStlModal(CadToStlConversionModal* modal)
{
    cadToStlModal = modal;
}

void CadAssetContextMenu::setCadToSvoModal(CadToSvoConversionModal* modal)
{
    cadToSvoModal = modal;
}

void CadAssetContextMenu::save(const CadAsset& asset)
{
    Library::SaveFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });

    std::string defaultName = asset.getName();
    if (std::filesystem::path(defaultName).extension().empty())
    {
        defaultName += ".step";
    }
    dialog.setSaveFileName(defaultName);

    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string savePath = dialog.getResultPath();
        if (std::filesystem::path(savePath).extension().empty())
        {
            savePath += ".step";
        }

        CADacomb::GUID targetGuid = asset.getGuid();
        Defer::getInstance().defer(
          [targetGuid, savePath]()
          {
              GatedDatabase::getInstance().executeAsync({
                {      "type",       "SaveStepAsset" },
                {   "version",                     1 },
                { "assetGuid", targetGuid.toString() },
                {  "filePath",              savePath }
              });
          });
    }
}
