#include "LoadVoxelOperation.h"

#include "Library/Voxel/BinaryVolume.h"
#include "Library/Voxel/MagicaVox/VoxFile.h"

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
}