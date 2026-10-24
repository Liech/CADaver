#include "VoxelAsset.h"

VoxelAsset::VoxelAsset(const std::string& nameInput)
  : Asset(nameInput, AssetType::Voxel)
{
}

VoxelAsset::~VoxelAsset() {}
