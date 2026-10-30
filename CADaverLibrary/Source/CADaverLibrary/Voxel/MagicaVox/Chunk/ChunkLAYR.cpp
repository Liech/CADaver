#include "ChunkLAYR.h"

#include "CADaverLibrary/Voxel/MagicaVox/IO/Reader.h"

namespace MagicaVoxImporter
{
    void ChunkLAYR::read(Reader own, Reader child)
    {
        Chunk::read(own, child);

        LayerID    = own.readInt();
        Attributes = own.readDict();
        own.readInt(); // reserved stuff
    }
}