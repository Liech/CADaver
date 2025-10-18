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
        result->dimension = res;
        result->origin    = aabb.first;
        result->size      = aabb.second;

        Voxelizer().voxelize(result->data, triangulation.vertices, triangulation.indices, aabb.first, aabb.first + aabb.second, res);
        return std::move(result);
    }
}