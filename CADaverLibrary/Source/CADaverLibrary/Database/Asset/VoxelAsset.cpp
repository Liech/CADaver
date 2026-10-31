#include "Database/Asset/VoxelAsset.h"

VoxelAsset::VoxelAsset() {}

VoxelAsset::~VoxelAsset() {}

std::string VoxelAsset::getType() const
{
    return "Voxel";
}

const CADacomb::SVO<bool>* VoxelAsset::getSVO() const
{
    return svo.get();
}

CADacomb::SVO<bool>* VoxelAsset::getSVO()
{
    return svo.get();
}

void VoxelAsset::setSVO(std::unique_ptr<CADacomb::SVO<bool>> newSvo)
{
    svo = std::move(newSvo);
    markModified();
}

const CADacomb::AABB3& VoxelAsset::getAABB() const
{
    return aabb;
}

void VoxelAsset::setAABB(const CADacomb::AABB3& newAABB)
{
    aabb = newAABB;
    markModified();
}

size_t VoxelAsset::getSizeInBytes() const
{
    size_t size = sizeof(VoxelAsset);
    size += getName().capacity();

    if (svo)
    {
        size += sizeof(CADacomb::SVO<bool>);
        size += svo->getRaw().capacity() * sizeof(CADacomb::SVONode<bool>);
        size += svo->getParents().capacity() * sizeof(uint32_t);
        size += svo->getFreeBlocks().capacity() * sizeof(uint32_t);
    }

    return size;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("VoxelAsset/getSizeInBytes")
{
    VoxelAsset asset;
    asset.setName("TestVoxel");
    CHECK(asset.getType() == "Voxel");
    REQUIRE(asset.getSizeInBytes() >= sizeof(VoxelAsset));

    auto svo = std::make_unique<CADacomb::SVO<bool>>(16, false);
    asset.setSVO(std::move(svo));
    REQUIRE(asset.getSizeInBytes() > sizeof(VoxelAsset));
}
#endif
