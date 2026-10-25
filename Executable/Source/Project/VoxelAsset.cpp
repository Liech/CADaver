#include "VoxelAsset.h"

VoxelAsset::VoxelAsset(const std::string& nameInput)
  : Asset(nameInput, AssetType::Voxel)
{
}

VoxelAsset::VoxelAsset(const std::string& nameInput, std::unique_ptr<CADacomb::SVO<bool>> svoInput, const CADacomb::AABB3& aabbInput)
  : Asset(nameInput, AssetType::Voxel)
  , svo(std::move(svoInput))
  , aabb(aabbInput)
{
}

VoxelAsset::VoxelAsset(const std::string& nameInput, const std::string& filePathInput, std::unique_ptr<CADacomb::SVO<bool>> svoInput, const CADacomb::AABB3& aabbInput)
  : Asset(nameInput, AssetType::Voxel)
  , svo(std::move(svoInput))
  , aabb(aabbInput)
  , filePath(filePathInput)
{
}

VoxelAsset::~VoxelAsset() {}

CADacomb::SVO<bool>* VoxelAsset::getSVO() const
{
    return svo.get();
}

void VoxelAsset::setSVO(std::unique_ptr<CADacomb::SVO<bool>> newSvo)
{
    svo   = std::move(newSvo);
    dirty = true;
}

const CADacomb::AABB3& VoxelAsset::getAABB() const
{
    return aabb;
}

void VoxelAsset::setAABB(const CADacomb::AABB3& newAABB)
{
    aabb  = newAABB;
    dirty = true;
}

const std::string& VoxelAsset::getFilePath() const
{
    return filePath;
}

void VoxelAsset::setFilePath(const std::string& path)
{
    filePath = path;
    dirty    = true;
}

bool VoxelAsset::isDirty() const
{
    return dirty;
}

void VoxelAsset::markClean()
{
    dirty = false;
}
