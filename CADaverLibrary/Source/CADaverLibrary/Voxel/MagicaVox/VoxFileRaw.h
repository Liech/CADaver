#pragma once

#include <string>
#include <memory>

namespace MagicaVoxImporter {
  class Chunk;
  
  class VoxFileRaw {
  public:
    VoxFileRaw();
    virtual ~VoxFileRaw();

    static std::unique_ptr<Chunk> read(const std::string& filename);
    static void write(const Chunk&, const std::string& filename);

  private:

  };
}