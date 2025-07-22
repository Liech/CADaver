#include "ChunkSIZE.h"

#include <string>
#include <iostream>

#include "Util/MagicaVox/IO/Reader.h"
#include "Util/MagicaVox/IO/Writer.h"

namespace MagicaVoxImporter {
  void ChunkSIZE::read(Reader own, Reader child) {
    Chunk::read(own, child);
    sizeX = own.readInt();
    sizeY = own.readInt();
    sizeZ = own.readInt();
  }

  void ChunkSIZE::print(int indentation) {
    Chunk::print(indentation);
    std::cout << std::string(indentation + 1, ' ') << sizeX << std::endl;
    std::cout << std::string(indentation + 1, ' ') << sizeY << std::endl;
    std::cout << std::string(indentation + 1, ' ') << sizeZ << std::endl;
  }

  void ChunkSIZE::write(std::vector<unsigned char>& file) const {
    Writer::write(file, getID());
    Writer::write(file, 12);//own content size
    Writer::write(file, 0); //child size
    Writer::write(file,sizeX);
    Writer::write(file,sizeY);
    Writer::write(file,sizeZ);
  }
}