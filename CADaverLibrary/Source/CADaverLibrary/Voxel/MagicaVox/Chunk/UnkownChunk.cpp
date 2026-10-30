#include "UnkownChunk.h"

#include "CADaverLibrary/Voxel/MagicaVox/IO/Reader.h"

namespace MagicaVoxImporter
{
    void UnkownChunk::read(Reader own, Reader child)
    {
        Chunk::read(own, child);
    }
}