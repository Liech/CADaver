#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter {
  //Chunk id 'PACK' : if it is absent, only one model in the file; only used for the animation in 0.98.2
  class ChunkPACK : public  Chunk {
  public:
    std::string getID() const override { return "PACK"; }
    void read(Reader own, Reader child) override;

    int numModels = 0;
  };
}