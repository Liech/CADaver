#pragma once
#include <array>

#include "Chunk.h"

namespace MagicaVoxImporter {
  //Transform Node Chunk
  class ChunknTRN : public  Chunk {
  public:
    std::string getID() const override { return "nTRN"; }
    void        read(Reader own, Reader child) override;

    int                ID;
    std::string        Name;
    bool               Hidden;
    int                ChildID;
    int                LayerID;
    std::string        Rotation = ""; //unfinished here
    std::string        Translation;  //unfinished here
  };
}