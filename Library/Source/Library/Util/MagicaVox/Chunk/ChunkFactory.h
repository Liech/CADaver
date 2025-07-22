#pragma once

#include <memory>
#include <string>


namespace MagicaVoxImporter {
  class Chunk;
  class ChunkFactory {
  public:
    static std::unique_ptr<Chunk> make(std::string id);
  };
}