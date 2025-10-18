#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace Library
{
    class BinaryVolume;
    class Triangulation;

    class LoadVoxelOperation
    {
      public:
        static std::unique_ptr<BinaryVolume> loadMagicaVox(const std::string& filename);
        static std::unique_ptr<BinaryVolume> voxelize(const Triangulation& triangulation, const glm::ivec3& resolution);
    };
}