#include "TriangulationAssetContextMenu.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Asset/TriangulationAsset.h"
#include "Library/Util/SaveFileDialog.h"

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
        if (ImGui::MenuItem("SVO"))
        {
            int targetIndex = getAssetIndex();
            Defer::getInstance().defer(
              [targetIndex]()
              {
                  GatedDatabase::getInstance().execute({
                    {       "type", "ConvertStlToSvo" },
                    {    "version",                 1 },
                    { "assetIndex",       targetIndex }
                  });
              });
        }
        ImGui::EndMenu();
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

        int targetIndex = getAssetIndex();
        if (targetIndex < 0)
        {
            targetIndex = GatedDatabase::getInstance().get().getProject().getSelectedIndex();
        }

        Defer::getInstance().defer(
          [targetIndex, savePath]()
          {
              std::ofstream fileStream(savePath, std::ios::binary);
              GatedDatabase::getInstance().execute(
                {
                  {       "type", "SaveStlAsset" },
                  {    "version",              1 },
                  { "assetIndex",    targetIndex }
              },
                nullptr,
                &fileStream);
          });

        asset.markClean();
    }
}
