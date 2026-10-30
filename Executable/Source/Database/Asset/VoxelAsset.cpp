#include "Database/Asset/VoxelAsset.h"

VoxelAsset::VoxelAsset(const std::string& nameInput)
  : Asset(nameInput, "Voxel")
{
}

VoxelAsset::VoxelAsset(const std::string& nameInput, std::unique_ptr<CADacomb::SVO<bool>> svoInput, const CADacomb::AABB3& aabbInput)
  : Asset(nameInput, "Voxel")
  , svo(std::move(svoInput))
  , aabb(aabbInput)
{
}

VoxelAsset::VoxelAsset(const std::string& nameInput, const std::string& filePathInput, std::unique_ptr<CADacomb::SVO<bool>> svoInput, const CADacomb::AABB3& aabbInput)
  : Asset(nameInput, "Voxel")
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

size_t VoxelAsset::getSizeInBytes() const
{
    size_t size = sizeof(VoxelAsset);
    size += getName().capacity();
    size += filePath.capacity();

    if (svo)
    {
        size += sizeof(CADacomb::SVO<bool>);
        size += svo->getRaw().capacity() * sizeof(CADacomb::SVONode<bool>);
        size += svo->getParents().capacity() * sizeof(uint32_t);
        size += svo->getFreeBlocks().capacity() * sizeof(uint32_t);
    }

    return size;
}

bool VoxelAsset::isDirty() const
{
    return dirty;
}

void VoxelAsset::markClean()
{
    dirty = false;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VoxelAsset/getSizeInBytes")
{
    VoxelAsset asset("TestVoxel");
    REQUIRE(asset.getSizeInBytes() >= sizeof(VoxelAsset));

    auto svo = std::make_unique<CADacomb::SVO<bool>>(16, false);
    asset.setSVO(std::move(svo));
    REQUIRE(asset.getSizeInBytes() > sizeof(VoxelAsset));
}
#endif
