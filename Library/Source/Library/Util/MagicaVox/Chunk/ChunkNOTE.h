#pragma once

#include <string>
#include <map>

#include "Chunk.h"

namespace MagicaVoxImporter {
  //Palette Note Chunk (defines palette color names)
  class ChunkNOTE : public  Chunk {
  public:
    std::string getID() const override { return "NOTE"; }
    void        read(Reader own, Reader child) override;

    std::vector<std::string> colorNames;
  };
}