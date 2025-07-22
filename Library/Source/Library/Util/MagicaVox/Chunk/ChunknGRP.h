#pragma once

#include "Chunk.h"
#include <map>

namespace MagicaVoxImporter {
  //Group Node Chunk
  class ChunknGRP : public Chunk {
  public:
    std::string getID() const override { return "nGRP"; }
    void        read(Reader own, Reader child) override;


    int                                ID;
    std::map<std::string, std::string> Attributes;
    std::vector<int>                   ChildNodes;
  };
}