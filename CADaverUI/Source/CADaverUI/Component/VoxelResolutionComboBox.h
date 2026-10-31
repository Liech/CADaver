#pragma once

#include <array>
#include <cstdint>

// Combo box component for selecting a voxel resolution.
class VoxelResolutionComboBox
{
  public:
    VoxelResolutionComboBox();
    virtual ~VoxelResolutionComboBox();

    bool render();

    uint32_t getResolution() const;
    void     setResolution(uint32_t resolution);

  private:
    int                                  selectedResolutionIndex = 2;
    static const std::array<uint32_t, 6> availableResolutions;
};
