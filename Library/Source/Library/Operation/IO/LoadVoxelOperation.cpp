#include "LoadVoxelOperation.h"

#include "Library/Triangle/Triangulation.h"
#include "Library/Voxel/BinaryVolume.h"
#include "Library/Voxel/MagicaVox/VoxFile.h"
#include "Library/Voxel/Voxelizer.h"

namespace Library
{
    std::unique_ptr<BinaryVolume> LoadVoxelOperation::loadMagicaVox(const std::string& filename)
    {
        auto vox          = MagicaVoxImporter::VoxFile::readBinary(filename);
        auto result       = std::make_unique<BinaryVolume>();
        result->data      = vox.first;
        result->dimension = glm::ivec3(vox.second[0], vox.second[1], vox.second[2]);
        result->origin    = glm::dvec3(0, 0, 0);
        result->size      = result->dimension;
        return std::move(result);
    }

    std::unique_ptr<BinaryVolume> LoadVoxelOperation::voxelize(const Triangulation& triangulation, const glm::ivec3& resolution)
    {
        std::unique_ptr<BinaryVolume> result = std::make_unique<BinaryVolume>();

        // only XYZ%8==0 allowed
        glm::u64vec3 res = resolution;
        res.x            = res.x + (8 - (res.x % 8));
        res.y            = res.y + (8 - (res.y % 8));
        res.z            = res.z + (8 - (res.z % 8));

        auto aabb         = triangulation.getAABB();
        auto voxelLength  = glm::dvec3(aabb.second.x / res.x, aabb.second.y / res.y, aabb.second.z / res.z);
        result->dimension = res;
        result->origin    = aabb.first - voxelLength * 2.0;
        result->size      = aabb.second + voxelLength * 4.0;

        Voxelizer().voxelize(result->data, triangulation.vertices, triangulation.indices, aabb.first, aabb.first + aabb.second, res);
        return std::move(result);
    }
}