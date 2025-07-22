#pragma once
#include "Chunk.h"
#include <map>

namespace MagicaVoxImporter {
  //Layer Chunk
  class ChunkLAYR : public Chunk {
  public:
    std::string getID() const override { return "LAYR"; }
    void        read(Reader own, Reader child) override;

    int LayerID;
    std::map<std::string, std::string> Attributes;
  };
}