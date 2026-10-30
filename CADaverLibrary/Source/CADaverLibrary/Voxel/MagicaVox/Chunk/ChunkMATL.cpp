#include "ChunkMATL.h"

#include "CADaverLibrary/Voxel/MagicaVox/IO/Reader.h"
#include <map>

namespace MagicaVoxImporter
{

    void ChunkMATL::read(Reader own, Reader child)
    {
        Chunk::read(own, child);

        MaterialID = own.readInt();
        Properties = own.readDict();
    }
}