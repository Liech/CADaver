#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Library
{
    class BinaryVolume
    {
      public:
        glm::ivec3        dimension;
        glm::dvec3        origin;
        glm::dvec3        size;
        std::vector<bool> data;
    };
}