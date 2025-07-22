#pragma once

#include <memory>
#include <array>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
//https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox-extension.txt
//https://paulbourke.net/dataformats/vox/
//https://github.com/jpaver/opengametools

namespace MagicaVoxImporter {
  class ChunkMAIN;

  class VoxFile {
  public:
    VoxFile(const std::string& filename);
    virtual ~VoxFile();

    static std::pair<std::unique_ptr<bool[]>, std::array<size_t, 3>> readBinary(const std::string& filename);
    static void writeBinary(bool* data, const std::array<size_t, 3>& size, const std::string& filename);

    std::vector<std::pair<std::vector<unsigned char>, std::array<size_t, 3>>> Models;
    std::array<std::array<unsigned char, 4>, 256>                             Colors;
    std::vector<std::map<std::string, std::string>>                           Materials;    
  };

}