#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter {
  struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
  };

  //Chunk id 'RGBA' : palette
  //Default Palette : if chunk 'RGBA' is absent
  class ChunkRGBA : public  Chunk {
  public:
    std::string getID() const override { return "RGBA"; }
    void read(Reader own, Reader child) override;

    color palette[256];

    static std::unique_ptr<ChunkRGBA> getDefault();
  };
}