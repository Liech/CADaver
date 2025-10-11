#pragma once

#include <memory>
#include <string>

namespace Library
{
    class BinaryVolume;

    class SaveVoxelOperation
    {
      public:
        static void saveMagicaVox(const BinaryVolume&, const std::string& filename);
    };
}