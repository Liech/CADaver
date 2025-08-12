#include "TriangulateOperation.h"

#include <map>
#include <tuple>

#include "CAD/CADShape.h"
#include "Triangle/Triangulation.h"

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Triangle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

namespace Library
{
    std::unique_ptr<Triangulation> TriangulateOperation::triangulate(const CADShape& shape, double precision)
    {
        std::unique_ptr<Triangulation> result = std::make_unique<Triangulation>();

        BRepMesh_IncrementalMesh mesher(shape.getData(), precision);
        mesher.Perform();
        std::map<std::tuple<double, double, double>, int> vertexMap;
        auto pointToTuple = [](const gp_Pnt& p) { return std::make_tuple(p.X(), p.Y(), p.Z()); };
        
        TopExp_Explorer faceExplorer(shape.getData(), TopAbs_FACE);
        for (; faceExplorer.More(); faceExplorer.Next())
        {
            TopoDS_Face face = TopoDS::Face(faceExplorer.Current());

            TopAbs_Orientation o        = face.Orientation();
            bool               reversed = o == TopAbs_REVERSED;
            TopLoc_Location    loc;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);
            if (!triangulation.IsNull())
            {
                size_t triangleAmount = triangulation->NbTriangles();
                size_t verticesAmount = triangulation->NbNodes();
                
                for (size_t i = 0; i < verticesAmount; i++)
                {
                    std::tuple<double, double, double> node = pointToTuple(triangulation->Node(i + 1));
                    if (vertexMap.find(node) == vertexMap.end())
                    {
                        vertexMap[node] = result->vertices.size();
                        result->vertices.push_back(glm::dvec3(std::get<0>(node), std::get<1>(node), std::get<2>(node)));
                    }
                }

                for (size_t i = 0; i < triangleAmount; i++)
                {
                    const Poly_Triangle& tri = triangulation->Triangle(i + 1);
                    int                  a, b, c;
                    tri.Get(a, b, c);
                    if (!reversed)
                    {
                        result->indices.push_back(vertexMap[pointToTuple(triangulation->Node(a))]);
                        result->indices.push_back(vertexMap[pointToTuple(triangulation->Node(c))]);
                        result->indices.push_back(vertexMap[pointToTuple(triangulation->Node(b))]);
                    }
                    else
                    {
                        result->indices.push_back(vertexMap[pointToTuple(triangulation->Node(a))]);
                        result->indices.push_back(vertexMap[pointToTuple(triangulation->Node(b))]);
                        result->indices.push_back(vertexMap[pointToTuple(triangulation->Node(c))]);
                    }
                }

            }
        }

        return std::move(result);
    }
}