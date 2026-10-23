#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace CADacomb
{
    class BinaryVolume;
    class Triangulation;
}

namespace Library
{
    class LoadVoxelOperation
    {
      public:
        static std::unique_ptr<CADacomb::BinaryVolume> loadMagicaVox(const std::string& filename);
        static std::unique_ptr<CADacomb::BinaryVolume> voxelize(const CADacomb::Triangulation& triangulation, const glm::ivec3& resolution);
    };
}
