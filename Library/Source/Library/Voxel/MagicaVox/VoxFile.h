#pragma once

#include <algorithm>
#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>

// https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
// https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox-extension.txt
// https://paulbourke.net/dataformats/vox/
// https://github.com/jpaver/opengametools

namespace MagicaVoxImporter
{
    class ChunkMAIN;

    class VoxFile
    {
      public:
        VoxFile(const std::string& filename);
        virtual ~VoxFile();

        static std::pair<std::vector<bool>, std::array<size_t, 3>> readBinary(const std::string& filename);
        static void                                                writeBinary(const std::vector<bool>& data, const std::array<size_t, 3>& size, const std::string& filename);

        std::vector<std::pair<std::vector<unsigned char>, std::array<size_t, 3>>> Models;
        std::array<std::array<unsigned char, 4>, 256>                             Colors;
        std::vector<std::map<std::string, std::string>>                           Materials;
    };

}