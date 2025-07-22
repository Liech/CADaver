#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter {
  //Chunk id 'SIZE' : model size
  class ChunkSIZE : public  Chunk {
  public:
    std::string getID() const override { return "SIZE"; }
    void        read(Reader own, Reader child) override;
    void        print(int indentation = 0) override;
    virtual void write(std::vector<unsigned char>&) const override;

    int sizeX = 0;
    int sizeY = 0;
    int sizeZ = 0;
  };
}