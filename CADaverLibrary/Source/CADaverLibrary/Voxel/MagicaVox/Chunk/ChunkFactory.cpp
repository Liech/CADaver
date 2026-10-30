#include "ChunkFactory.h"
#include "Chunk.h"
#include <stdexcept>

// No fancy template factory. I hope it is easier to understand this way
#include "ChunkLAYR.h"
#include "ChunkMAIN.h"
#include "ChunkMATL.h"
#include "ChunkNOTE.h"
#include "ChunkPACK.h"
#include "ChunkRCAM.h"
#include "ChunkRGBA.h"
#include "ChunkSIZE.h"
#include "ChunkXYZI.h"
#include "ChunknGRP.h"
#include "ChunknSHP.h"
#include "ChunknTRN.h"
#include "ChunkrOBJ.h"
#include "UnkownChunk.h"

namespace MagicaVoxImporter
{
    std::unique_ptr<Chunk> ChunkFactory::make(std::string id)
    {
        if (id == "LAYR")
            return std::make_unique<ChunkLAYR>();
        else if (id == "MAIN")
            return std::make_unique<ChunkMAIN>();
        else if (id == "MATL")
            return std::make_unique<ChunkMATL>();
        else if (id == "nGRP")
            return std::make_unique<ChunknGRP>();
        else if (id == "nSHP")
            return std::make_unique<ChunknSHP>();
        else if (id == "nTRN")
            return std::make_unique<ChunknTRN>();
        else if (id == "PACK")
            return std::make_unique<ChunkPACK>();
        else if (id == "RGBA")
            return std::make_unique<ChunkRGBA>();
        else if (id == "SIZE")
            return std::make_unique<ChunkSIZE>();
        else if (id == "XYZI")
            return std::make_unique<ChunkXYZI>();
        else if (id == "rOBJ")
            return std::make_unique<ChunkrOBJ>();
        else if (id == "rCAM")
            return std::make_unique<ChunkRCAM>();
        else if (id == "NOTE")
            return std::make_unique<ChunkNOTE>();
        else
            return std::make_unique<UnkownChunk>(); // unkown chunk
            //throw std::runtime_error("Unkown ID: " + id);
    }
}