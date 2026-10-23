#pragma once

#include <memory>
#include <string>

namespace CADacomb
{
    class BinaryVolume;
}

namespace Library
{
    class SaveVoxelOperation
    {
      public:
        static void saveMagicaVox(const CADacomb::BinaryVolume&, const std::string& filename);
    };
}
