#include "CadAssetContextMenu.h"

#include "Commands/GatedDatabase.h"
#include "Database/Asset/CadAsset.h"
#include "Library/Util/SaveFileDialog.h"

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

        int selectedIndex = GatedDatabase::getInstance().get().getProject().getSelectedIndex();
        GatedDatabase::getInstance().execute({
          {       "type", "SaveStepAsset" },
          {    "version",               1 },
          { "assetIndex",   selectedIndex },
          {   "filePath",        savePath }
        });

        asset.setFilePath(savePath);
        asset.markClean();
    }
}
