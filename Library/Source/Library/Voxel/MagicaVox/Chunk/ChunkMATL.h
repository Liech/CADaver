#pragma once

#include "Chunk.h"
#include <map>

namespace MagicaVoxImporter {
  //Material Chunk
  class ChunkMATL : public Chunk {
  public:
    std::string getID() const override { return "MATL"; }
    void        read(Reader own, Reader child) override;

    int MaterialID;
    std::map<std::string, std::string> Properties;
  };
}