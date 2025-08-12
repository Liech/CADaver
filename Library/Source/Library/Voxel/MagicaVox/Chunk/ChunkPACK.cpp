#include "ChunkPACK.h"
#include "Voxel/MagicaVox/IO/Reader.h"

namespace MagicaVoxImporter
{
    void ChunkPACK::read(Reader own, Reader child)
    {
        Chunk::read(own, child);
        numModels = own.readInt();
    }
}