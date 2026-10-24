#pragma once

#include "Asset.h"

#include <string>

class VoxelAsset : public Asset
{
  public:
    VoxelAsset(const std::string& name);
    virtual ~VoxelAsset();
};
