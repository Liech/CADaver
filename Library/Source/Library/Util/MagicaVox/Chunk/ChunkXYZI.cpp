#include "ChunkXYZI.h"
#include "Util/MagicaVox/IO/Reader.h"
#include "Util/MagicaVox/IO/Writer.h"

#include <iostream>
#include <string>

namespace MagicaVoxImporter {
  void ChunkXYZI::read(Reader own, Reader child) {
    Chunk::read(own, child);
    numVoxels = own.readInt();
    content.resize(numVoxels);

    for (int i = 0; i < numVoxels; i++) {
      voxel current;
      std::vector<unsigned char> voxelContent = own.readChars(4);
      current.X = voxelContent[0];
      current.Y = voxelContent[1];
      current.Z = voxelContent[2];
      current.value = voxelContent[3];
      content[i] = current;
    }
  }

  void ChunkXYZI::print(int indentation) {
    Chunk::print(indentation);
    std::cout << std::string(indentation + 1, ' ') << content.size() << std::endl;
  }

  void ChunkXYZI::write(std::vector<unsigned char>& file) const {
    Writer::write(file, getID());
    Writer::write(file, 4 + 4 * numVoxels);//own content size
    Writer::write(file, 0); //child size
    Writer::write(file, numVoxels);

    for (size_t i = 0; i < numVoxels; i++) {
      auto& v = content[i];
      file.push_back(v.X);
      file.push_back(v.Y);
      file.push_back(v.Z);
      file.push_back(v.value);
    }
  }
}