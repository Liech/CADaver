#pragma once

#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <glm/glm.hpp>
#include <vector>

namespace Library
{
    class cocoon
    {
      public:
        static TopoDS_Face convert(const TopoDS_Wire& wire);
        static TopoDS_Face convert(const std::vector<glm::dvec3>& wire);

      private:
        static std::vector<std::vector<glm::dvec3>> cutWire(const std::vector<glm::dvec3>& wire);
        static glm::ivec2                          getGridSize(const std::vector<std::vector<glm::dvec3>>& quarters);
        static std::vector<std::vector<glm::dvec3>> fixSections(std::vector<std::vector<glm::dvec3>> sections, glm::ivec2 resolution);
        static std::vector<std::vector<glm::dvec3>> calculateCoon(std::vector<std::vector<glm::dvec3>> sections, glm::ivec2 resolution);
        static TopoDS_Face                         toFace(std::vector<std::vector<glm::dvec3>> grid);
        static std::string                         grid2string(const std::vector<std::vector<glm::dvec3>>& grid);
        static TopoDS_Face                         fallback(const std::vector<glm::dvec3>&);
    };
}