#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Library
{
    class BinaryVolume
    {
      public:
        glm::u64vec3      dimension;
        glm::dvec3        origin;
        glm::dvec3        size;
        std::vector<bool> data;
    };
}