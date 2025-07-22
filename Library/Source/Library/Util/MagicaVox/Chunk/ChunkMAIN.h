#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter {
  //main can do nothing. only having children
  class ChunkMAIN : public Chunk {
  public:
    std::string getID() const override { return "MAIN"; }

    virtual void write(std::vector<unsigned char>&) const override;
  };
}