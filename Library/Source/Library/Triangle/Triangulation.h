#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Library
{
    class Triangulation
    {
      public:
        Triangulation();
        virtual ~Triangulation();

        std::vector<glm::dvec3> vertices;
        std::vector<int>        indices;

        void saveAsSTL(const std::string& filename) const;
    };
}