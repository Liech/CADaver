#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class TopoDS_Face;
class TopoDS_Edge;
class gp_Pnt;

namespace Library
{
    class CADShape;
    class Triangulation;

    class mesh2cad_cluster
    {
      public:
        mesh2cad_cluster();
        virtual ~mesh2cad_cluster();

        static std::unique_ptr<CADShape> convert(const Triangulation& mesh, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction);

      //private:
        static std::vector<TopoDS_Face> Clusters2BREP(const std::vector<std::vector<size_t>>&          clusters,
                                                      const std::vector<std::vector<size_t>>&          borders,
                                                      const Triangulation&                             mesh,
                                                      std::map<size_t, gp_Pnt>&                        vcache,
                                                      std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache);
    };
}