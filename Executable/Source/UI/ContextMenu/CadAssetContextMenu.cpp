#include "CadAssetContextMenu.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/CadAsset.h"
#include "Library/Util/SaveFileDialog.h"
#include "UI/Converter/CadToStlConversionModal.h"
#include "UI/Converter/CadToSvoConversionModal.h"

#include <filesystem>
#include <imgui.h>

CadAssetContextMenu::CadAssetContextMenu() {}

CadAssetContextMenu::~CadAssetContextMenu() {}

void CadAssetContextMenu::render(Asset& asset)
{
    auto cadAsset = dynamic_cast<CadAsset*>(&asset);
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
                cadToStlModal->open(getAssetGuid(), cadAsset->getName());
            }
        }
        if (ImGui::MenuItem("SVO..."))
        {
            if (cadToSvoModal)
            {
                cadToSvoModal->open(getAssetGuid(), cadAsset->getName());
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

void CadAssetContextMenu::save(CadAsset& asset)
{
    if (!asset.getCadShape())
    {
        return;
    }

    Library::SaveFileDialog dialog;
    dialog.addFilter("STEP File", { "step", "stp" });

    if (!asset.getFilePath().empty())
    {
        std::filesystem::path existingPath(asset.getFilePath());
        dialog.setDefaultPath(existingPath.parent_path().string());
        dialog.setSaveFileName(existingPath.filename().string());
    }
    else
    {
        std::string defaultName = asset.getName();
        if (std::filesystem::path(defaultName).extension().empty())
        {
            defaultName += ".step";
        }
        dialog.setSaveFileName(defaultName);
    }

    dialog.execute();

    if (!dialog.isCancled() && !dialog.getResultPath().empty())
    {
        std::string savePath = dialog.getResultPath();
        if (std::filesystem::path(savePath).extension().empty())
        {
            savePath += ".step";
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
                  GatedDatabase::getInstance().execute({
                    {      "type",       "SaveStepAsset" },
                    {   "version",                     1 },
                    { "assetGuid", targetGuid.toString() },
                    {  "filePath",              savePath }
                  });
              });
        }
    }
}
