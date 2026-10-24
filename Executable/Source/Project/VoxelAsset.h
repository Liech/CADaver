#pragma once

#include "Asset.h"

#include <string>

// Headless data asset representing volumetric voxel geometry.
class VoxelAsset : public Asset
{
  public:
    VoxelAsset(const std::string& name);
    virtual ~VoxelAsset();
};
