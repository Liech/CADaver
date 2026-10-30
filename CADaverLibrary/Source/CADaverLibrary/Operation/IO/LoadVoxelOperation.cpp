#include "LoadVoxelOperation.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Volume/Flat/BinaryVolume.h"
#include "CADacomb/Volume/Flat/Voxelizer.h"
#include "CADaverLibrary/Voxel/MagicaVox/VoxFile.h"

namespace Library
{
    std::unique_ptr<CADacomb::BinaryVolume> LoadVoxelOperation::loadMagicaVox(const std::string& filename)
    {
        auto vox          = MagicaVoxImporter::VoxFile::readBinary(filename);
        auto result       = std::make_unique<CADacomb::BinaryVolume>();
        result->data      = vox.first;
        result->dimension = glm::ivec3(vox.second[0], vox.second[1], vox.second[2]);
        result->origin    = glm::dvec3(0, 0, 0);
        result->size      = result->dimension;
        return std::move(result);
    }

    std::unique_ptr<CADacomb::BinaryVolume> LoadVoxelOperation::voxelize(const CADacomb::Triangulation& triangulation, const glm::ivec3& resolution)
    {
        std::unique_ptr<CADacomb::BinaryVolume> result = std::make_unique<CADacomb::BinaryVolume>();

        glm::u64vec3 res = resolution;
        res.x            = res.x + (8 - (res.x % 8));
        res.y            = res.y + (8 - (res.y % 8));
        res.z            = res.z + (8 - (res.z % 8));

        auto       aabb        = triangulation.getAABB();
        auto       voxelLength = glm::dvec3(aabb.second.x / res.x, aabb.second.y / res.y, aabb.second.z / res.z);
        glm::dvec3 origin      = aabb.first - voxelLength * 2.0;
        glm::dvec3 size        = aabb.second + voxelLength * 4.0;

        return CADacomb::Voxelizer().voxelize(triangulation.vertices, triangulation.indices, origin, size, res);
    }
}
