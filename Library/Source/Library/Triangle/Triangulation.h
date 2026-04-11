#pragma once

#include <glm/glm.hpp>
#include <map>
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

        std::pair<glm::dvec3, glm::dvec3>                        getAABB() const;
        std::map<std::pair<size_t, size_t>, std::vector<size_t>> getAdjacency() const;
        glm::dvec3                                               getFaceNormal(size_t faceIndex) const;

        void                                  saveAsSTL(const std::string& filename) const;
        static std::unique_ptr<Triangulation> fromSTLFile(const std::string& filename);
    };
}

/*

    class Triangulation
    {
      public:
        std::vector<glm::dvec3> vertices;
        std::vector<size_t>     indices;
    };
*/