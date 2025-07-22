#include "Chunk.h"

#include "Util/MagicaVox/IO/Reader.h"
#include "ChunkFactory.h"

#include <iostream>

namespace MagicaVoxImporter {
  void Chunk::read(Reader own, Reader child) {
    //handle children
    while (!child.endOfBufferReached()) {
      addChild(child.readChunk());
    }
  }

  size_t Chunk::numberOfChilds() const {
    return _childs.size();
  }

  const Chunk& Chunk::getChild(size_t i) const {
    return *_childs[i];
  }

  void Chunk::addChild(std::unique_ptr<Chunk> newchild) {
    _childs.push_back(std::move(newchild));
  }

  void Chunk::print(int indentation) {
    std::cout << std::string(indentation, ' ') << getID() << std::endl;
    for (int i = 0; i < _childs.size(); i++)
      _childs[i]->print(indentation + 1);
  }

  void Chunk::write(std::vector<unsigned char>& file) const {
  }
}