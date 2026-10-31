#include "UI/ContextMenu/CadAssetContextMenu.h"

#include "Database/Asset/CadAsset.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Dialog/SaveFileDialog.h"
#include "Windows/Conversion/CadToStlConversionModal.h"
#include "Windows/Conversion/CadToSvoConversionModal.h"
#include "Windows/WindowContainer.h"

#include <filesystem>
#include <imgui.h>
#include <memory>

CadAssetContextMenu::CadAssetContextMenu() {}

CadAssetContextMenu::~CadAssetContextMenu() {}

void CadAssetContextMenu::render(GatedDatabase& gatedDatabase, WindowContainer& windowContainer, const Asset& asset)
{
    auto cadAsset = dynamic_cast<const CadAsset*>(&asset);
    if (!cadAsset)
    {
        return;
    }

    if (ImGui::MenuItem("Save"))
    {
        save(gatedDatabase, *cadAsset);
    }

    if (ImGui::BeginMenu("Converter"))
    {
        if (ImGui::MenuItem("STL..."))
        {
            windowContainer.open("CadToStlConversionModal");
            auto modal = std::dynamic_pointer_cast<CadToStlConversionModal>(windowContainer.get("CadToStlConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(cadAsset->getGuid());
            }
        }
        if (ImGui::MenuItem("SVO..."))
        {
            windowContainer.open("CadToSvoConversionModal");
            auto modal = std::dynamic_pointer_cast<CadToSvoConversionModal>(windowContainer.get("CadToSvoConversionModal"));
            if (modal)
            {
                modal->setTargetAsset(cadAsset->getGuid());
            }
        }
        ImGui::EndMenu();
    }
}

void CadAssetContextMenu::save(GatedDatabase& gatedDatabase, const CadAsset& asset)
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
          [&gatedDatabase, targetGuid, savePath]()
          {
              gatedDatabase.executeAsync({
                {      "type",       "SaveStepAsset" },
                {   "version",                     1 },
                { "assetGuid", targetGuid.toString() },
                {  "filePath",              savePath }
              });
          });
    }
}
