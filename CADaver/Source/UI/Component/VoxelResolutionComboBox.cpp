#include "VoxelResolutionComboBox.h"

#include <imgui.h>

const std::array<uint32_t, 6> VoxelResolutionComboBox::availableResolutions = { 16, 32, 64, 128, 256, 512 };

VoxelResolutionComboBox::VoxelResolutionComboBox() {}

VoxelResolutionComboBox::~VoxelResolutionComboBox() {}

bool VoxelResolutionComboBox::render()
{
    const char* items = "16\00032\00064\000128\000256\000512\000\000";
    if (ImGui::Combo("Voxel Resolution", &selectedResolutionIndex, items))
    {
        return true;
    }
    return false;
}

uint32_t VoxelResolutionComboBox::getResolution() const
{
    if (selectedResolutionIndex >= 0 && static_cast<size_t>(selectedResolutionIndex) < availableResolutions.size())
    {
        return availableResolutions[static_cast<size_t>(selectedResolutionIndex)];
    }
    return 64;
}

void VoxelResolutionComboBox::setResolution(uint32_t resolution)
{
    for (size_t i = 0; i < availableResolutions.size(); i++)
    {
        if (availableResolutions[i] == resolution)
        {
            selectedResolutionIndex = static_cast<int>(i);
            return;
        }
    }
    selectedResolutionIndex = 2;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VoxelResolutionComboBox/DefaultResolution")
{
    VoxelResolutionComboBox combo;
    REQUIRE(combo.getResolution() == 64);
}

TEST_CASE("VoxelResolutionComboBox/SetResolution")
{
    VoxelResolutionComboBox combo;
    combo.setResolution(256);
    REQUIRE(combo.getResolution() == 256);

    combo.setResolution(16);
    REQUIRE(combo.getResolution() == 16);

    combo.setResolution(999);
    REQUIRE(combo.getResolution() == 64);
}
#endif
