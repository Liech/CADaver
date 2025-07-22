#pragma once

#include <glm/glm.hpp>
#include <vector>

class BinaryVolume
{
  public:
    glm::ivec3        dimension;
    std::vector<bool> data;
};