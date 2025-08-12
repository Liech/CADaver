#include "SaveVoxelOperation.h"

#include "Library/Voxel/BinaryVolume.h"
#include "Library/Voxel/MagicaVox/VoxFile.h"

namespace Library
{
    void SaveVoxelOperation::saveMagicaVox(const BinaryVolume& volume, const std::string& filename)
    {
        auto dim = std::array<size_t, 3>{ (size_t)volume.dimension.x, (size_t)volume.dimension.y, (size_t)volume.dimension.z };
        MagicaVoxImporter::VoxFile::writeBinary(volume.data, dim, filename);
    }
}