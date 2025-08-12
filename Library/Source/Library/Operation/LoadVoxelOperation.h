#pragma once

#include <memory>
#include <string>

namespace Library
{
    class BinaryVolume;

    class LoadVoxelOperation
    {
      public:
        static std::unique_ptr<BinaryVolume> loadMagicaVox(const std::string& filename);
    };
}