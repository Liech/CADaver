#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Library
{
    class Triangulation
    {
      public:
        Triangulation();
        virtual ~Triangulation();

        std::vector<glm::dvec3> vertices;
        std::vector<size_t>     indices;

        std::pair<glm::dvec3, glm::dvec3> getAABB() const;

        void                                  saveAsSTL(const std::string& filename) const;
        static std::unique_ptr<Triangulation> fromSTLFile(const std::string& filename);
    };
}