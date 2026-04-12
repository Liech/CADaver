#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

class TopoDS_Face;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Vertex;
class TopoDS_Shape;
class TopoDS_Solid;
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
        static std::unique_ptr<CADShape> convert(const Triangulation& mesh, double threshold);

        // private:
        static std::vector<TopoDS_Wire>             Borders2Wires(const std::vector<std::vector<size_t>>&           borders,
                                                                  const Triangulation&                              mesh,
                                                                  std::map<size_t, TopoDS_Vertex>&                  vcache,
                                                                  std::map<std::pair<size_t, size_t>, TopoDS_Edge>& ecache);
        static std::vector<std::vector<glm::dvec3>> Borders2EdgeNormals(const std::vector<std::vector<size_t>>& borders,
                                                                        const std::vector<std::vector<size_t>>& clusters, // Neu: Die Cluster-Information
                                                                        const Triangulation&                    mesh);

        static std::vector<TopoDS_Face> Cluster2Faces(const std::vector<TopoDS_Wire>&         wires,
                                                      const std::vector<std::vector<size_t>>& clusters,
                                                      const std::vector<std::vector<glm::dvec3>>& edgeNormals,
                                                      const Triangulation&                    mesh,
                                                      std::map<size_t, TopoDS_Vertex>&        vcache);
        static std::vector<TopoDS_Face>        Cluster2Coon(const std::vector<TopoDS_Wire>& wires, const Triangulation& mesh);

        static TopoDS_Shape             StitchFaces(const std::vector<TopoDS_Face>& faces, double tolerance = 1e-6);
        static TopoDS_Solid             MakeSolid(const TopoDS_Shape& stitchedShell);

        inline static std::string report = "";
    };
}