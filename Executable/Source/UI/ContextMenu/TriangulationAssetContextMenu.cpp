#include "TriangulationAssetContextMenu.h"

#include "Commands/GatedDatabase.h"
#include "Library/Util/SaveFileDialog.h"
#include "Project/TriangulationAsset.h"

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

        std::ofstream fileStream(savePath, std::ios::binary);
        int           selectedIndex = GatedDatabase::getInstance().get().getProject().getSelectedIndex();
        GatedDatabase::getInstance().execute(
          {
            {       "type", "SaveStlAsset" },
            {    "version",              1 },
            { "assetIndex",  selectedIndex }
        },
          nullptr,
          &fileStream);

        asset.markClean();
    }
}
