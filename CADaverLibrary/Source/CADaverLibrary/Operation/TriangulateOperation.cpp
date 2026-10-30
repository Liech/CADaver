#include "TriangulateOperation.h"

#include <map>
#include <tuple>

#include "CADaverLibrary/CAD/CADShape.h"
#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Volume/Flat/BinaryVolume.h"
#include "CADacomb/Volume/Flat/BlockyVoxelTriangulation.h"
#include "CADacomb/Volume/Flat/MarchingCubes.h"

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Poly_Triangle.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

namespace Library
{
    std::unique_ptr<CADacomb::Triangulation> TriangulateOperation::triangulate(const CADShape& shape, double precision)
    {
        std::unique_ptr<CADacomb::Triangulation> result = std::make_unique<CADacomb::Triangulation>();

        BRepTools::Clean(shape.getData());
        BRepMesh_IncrementalMesh mesher(shape.getData(), precision);
        mesher.Perform();
        std::map<std::tuple<double, double, double>, int> vertexMap;
        auto                                              pointToTuple = [](const gp_Pnt& p) { return std::make_tuple(p.X(), p.Y(), p.Z()); };

        TopExp_Explorer faceExplorer(shape.getData(), TopAbs_FACE);
        for (; faceExplorer.More(); faceExplorer.Next())
        {
            TopoDS_Face face = TopoDS::Face(faceExplorer.Current());

            TopAbs_Orientation o        = face.Orientation();
            bool               reversed = (o == TopAbs_REVERSED);
            TopLoc_Location    loc;
            Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(face, loc);
            if (!triangulation.IsNull())
            {
                bool    hasLoc = !loc.IsIdentity();
                gp_Trsf trsf;
                if (hasLoc)
                {
                    trsf = loc.Transformation();
                    if (trsf.IsNegative())
                    {
                        reversed = !reversed;
                    }
                }

                size_t triangleAmount = triangulation->NbTriangles();
                size_t verticesAmount = triangulation->NbNodes();

                std::vector<int> faceVertexMap(verticesAmount + 1);
                for (size_t i = 1; i <= verticesAmount; i++)
                {
                    gp_Pnt p = triangulation->Node(static_cast<int>(i));
                    if (hasLoc)
                    {
                        p.Transform(trsf);
                    }
                    std::tuple<double, double, double> node = pointToTuple(p);
                    auto                               it   = vertexMap.find(node);
                    if (it == vertexMap.end())
                    {
                        int newIndex     = static_cast<int>(result->vertices.size());
                        vertexMap[node]  = newIndex;
                        faceVertexMap[i] = newIndex;
                        result->vertices.push_back(glm::dvec3(p.X(), p.Y(), p.Z()));
                    }
                    else
                    {
                        faceVertexMap[i] = it->second;
                    }
                }

                for (size_t i = 1; i <= triangleAmount; i++)
                {
                    const Poly_Triangle& tri = triangulation->Triangle(static_cast<int>(i));
                    int                  a, b, c;
                    tri.Get(a, b, c);
                    if (!reversed)
                    {
                        result->indices.push_back(faceVertexMap[a]);
                        result->indices.push_back(faceVertexMap[b]);
                        result->indices.push_back(faceVertexMap[c]);
                    }
                    else
                    {
                        result->indices.push_back(faceVertexMap[a]);
                        result->indices.push_back(faceVertexMap[c]);
                        result->indices.push_back(faceVertexMap[b]);
                    }
                }
            }
        }

        return std::move(result);
    }

    std::unique_ptr<CADacomb::Triangulation> TriangulateOperation::triangulateRound(const CADacomb::BinaryVolume& shape)
    {
        std::vector<glm::dvec3>                  rawResult = CADacomb::MarchingCubes::polygonize(shape.data, shape.origin, glm::dvec3(1, 1, 1), shape.dimension);
        // initially work with soup
        std::unique_ptr<CADacomb::Triangulation> result = std::make_unique<CADacomb::Triangulation>();
        std::reverse(rawResult.begin(), rawResult.end());
        result->indices.resize(rawResult.size());
        for (size_t i = 0; i < rawResult.size(); i++)
            result->indices[i] = i;
        result->vertices = rawResult;

        return std::move(result);
    }

    std::unique_ptr<CADacomb::Triangulation> TriangulateOperation::triangulateBlocky(const CADacomb::BinaryVolume& shape)
    {
        return std::move(CADacomb::BlockyVoxelTriangulation(shape).triangulate());
    }
}