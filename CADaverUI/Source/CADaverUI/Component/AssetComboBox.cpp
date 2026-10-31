#include "Component/AssetComboBox.h"

#include "Database/Asset.h"
#include "Database/Project.h"

#include <imgui.h>
#include <memory>
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

bool AssetComboBox::render(const Project& project)
{
    std::vector<std::shared_ptr<const Asset>> matchingAssets;
    for (const auto& asset : project.getAssets())
    {
        if (!asset)
        {
            continue;
        }
        if (filterType.empty() || asset->getType() == filterType)
        {
            matchingAssets.push_back(asset);
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

    std::shared_ptr<const Asset> currentAsset = nullptr;
    for (const auto& asset : matchingAssets)
    {
        if (asset->getGuid() == selectedGuid)
        {
            currentAsset = asset;
            break;
        }
    }

    if (!currentAsset)
    {
        currentAsset = matchingAssets.front();
        selectedGuid = currentAsset->getGuid();
    }

    bool        changed      = false;
    std::string previewLabel = currentAsset->getName();
    if (ImGui::BeginCombo("Target Asset", previewLabel.c_str()))
    {
        for (const auto& asset : matchingAssets)
        {
            bool        isSelected = (asset->getGuid() == selectedGuid);
            std::string itemLabel  = asset->getName() + "##" + asset->getGuid().toString();
            if (ImGui::Selectable(itemLabel.c_str(), isSelected))
            {
                if (selectedGuid != asset->getGuid())
                {
                    selectedGuid = asset->getGuid();
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
