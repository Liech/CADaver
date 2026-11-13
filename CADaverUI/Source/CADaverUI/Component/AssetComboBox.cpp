#include "Component/AssetComboBox.h"

#include "Execution/GatedDatabase.h"

#include <imgui.h>
#include <utility>
#include <vector>

AssetComboBox::AssetComboBox()
  : selectedGuid("")
{
}

AssetComboBox::AssetComboBox(const std::string& filterTypeInput)
  : selectedGuid("")
  , filterType(filterTypeInput)
{
}

AssetComboBox::~AssetComboBox() {}

bool AssetComboBox::render(GatedDatabase& gatedDatabase)
{
    Polyglot::Node response   = gatedDatabase.execute({
      {    "type", "GetAssets" },
      { "version",           1 }
    });
    nlohmann::json assetsJson = response.toJson().value("assets", nlohmann::json::array());

    std::vector<std::pair<CADacomb::GUID, std::string>> matchingAssets;
    for (const auto& item : assetsJson)
    {
        std::string itemType = item.value("type", "");
        if (filterType.empty() || itemType == filterType)
        {
            matchingAssets.emplace_back(CADacomb::GUID(item.value("assetGuid", "")), item.value("name", ""));
        }
    }

    if (matchingAssets.empty())
    {
        selectedGuid = CADacomb::GUID("");
        ImGui::BeginDisabled();
        if (ImGui::BeginCombo("Target Asset", "No matching assets"))
        {
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        return false;
    }

    int currentIdx = -1;
    for (size_t i = 0; i < matchingAssets.size(); i++)
    {
        if (matchingAssets[i].first == selectedGuid)
        {
            currentIdx = static_cast<int>(i);
            break;
        }
    }

    if (currentIdx < 0)
    {
        currentIdx   = 0;
        selectedGuid = matchingAssets.front().first;
    }

    bool        changed      = false;
    std::string previewLabel = matchingAssets[static_cast<size_t>(currentIdx)].second;
    if (ImGui::BeginCombo("Target Asset", previewLabel.c_str()))
    {
        for (const auto& asset : matchingAssets)
        {
            bool        isSelected = (asset.first == selectedGuid);
            std::string itemLabel  = asset.second + "##" + asset.first.toString();
            if (ImGui::Selectable(itemLabel.c_str(), isSelected))
            {
                if (selectedGuid != asset.first)
                {
                    selectedGuid = asset.first;
                    changed      = true;
                }
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    return changed;
}

const CADacomb::GUID& AssetComboBox::getSelectedGuid() const
{
    return selectedGuid;
}

void AssetComboBox::setSelectedGuid(const CADacomb::GUID& guid)
{
    selectedGuid = guid;
}

const std::string& AssetComboBox::getFilterType() const
{
    return filterType;
}

void AssetComboBox::setFilterType(const std::string& filterTypeInput)
{
    filterType = filterTypeInput;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AssetComboBox/DefaultState")
{
    AssetComboBox combo("Cad");
    CHECK(combo.getFilterType() == "Cad");
    CHECK(combo.getSelectedGuid() == CADacomb::GUID(""));

    CADacomb::GUID target = CADacomb::GUID::random();
    combo.setSelectedGuid(target);
    CHECK(combo.getSelectedGuid() == target);
}
#endif
