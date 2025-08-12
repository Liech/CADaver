#include "ChunkNOTE.h"
#include "Voxel/MagicaVox/IO/Reader.h"

#include <iostream>
#include <string>

namespace MagicaVoxImporter
{
    void ChunkNOTE::read(Reader own, Reader child)
    {
        Chunk::read(own, child);

        int amountColors = own.readInt();

        colorNames.resize(amountColors);
        for (int i = 0; i < amountColors; i++)
        {
            colorNames[i] = own.readString();
        }
    }
}