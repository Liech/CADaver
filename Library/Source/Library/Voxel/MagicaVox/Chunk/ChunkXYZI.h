#pragma once

#include "Chunk.h"

namespace MagicaVoxImporter {
  struct voxel {
    unsigned char X;
    unsigned char Y;
    unsigned char Z;
    unsigned char value;
  };

  //Chunk id 'XYZI' : model voxels, paired with the SIZE chunk
  class ChunkXYZI : public  Chunk {
  public:
    std::string getID() const override { return "XYZI"; }
    void        read(Reader own, Reader child) override;
    void        print(int indentation = 0) override;
    virtual void write(std::vector<unsigned char>&) const override;

    int                numVoxels = 0;
    std::vector<voxel> content;
  };
}