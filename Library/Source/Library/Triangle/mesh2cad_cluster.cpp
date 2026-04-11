#include "mesh2cad_cluster.h"

#include "CAD/CADShape.h"
#include "CAD/CADShapeFactory.h"
#include "Clustering.h"
#include "HalfEdge/HalfEdge.h"
#include "HalfEdge/mesh2halfedge.h"
#include "Triangulation.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs_ShapeEnum.hxx> // For TopAbs_SHELL, etc.
#include <TopExp_Explorer.hxx>  // For TopExp_Explorer
#include <TopoDS.hxx>           // For the TopoDS casting utility
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <algorithm>
#include <gp_Pnt.hxx>
#include <map>
#include <set>
#include <vector>
#include <BRepLib.hxx>

namespace Library
{
    mesh2cad_cluster::mesh2cad_cluster() {}
    mesh2cad_cluster::~mesh2cad_cluster() {}

    std::unique_ptr<CADShape> mesh2cad_cluster::convert(const Triangulation& mesh, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction)
    {
        // 0. Preperation
        std::vector<std::vector<size_t>> cluster  = Clustering::cluster_withoutHoles(mesh, growFunction);
        auto                             halfedge = mesh2halfedge::convert(mesh);
        std::vector<std::vector<size_t>> borders;

        for (const auto& c : cluster)
            borders.push_back(Clustering::findBorders(*halfedge, c)[0]); // withoutHoles ensures .size()==1

        // 1. Convert Mesh Clusters to B-Rep Faces
        std::map<size_t, gp_Pnt>                         vcache;
        std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;
        auto                                             breps = Clusters2BREP(cluster, borders,mesh, vcache, ecache);

        //
        //   BRepOffsetAPI_MakeFilling
        // 2. Defining the Boundaries (Wires)
        // 3. Stitching Faces into a Shell
        // 4. Creating the Solid
        return nullptr;
    }

    std::vector<TopoDS_Face> mesh2cad_cluster::Clusters2BREP(const std::vector<std::vector<size_t>>&          clusters,
                                                             const std::vector<std::vector<size_t>>&          borders,
                                                             const Triangulation&                             mesh,
                                                             std::map<size_t, gp_Pnt>&                        vcache,
                                                             std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache)
    {
        std::vector<TopoDS_Face> resultFaces;

        for (size_t clusterId = 0; clusterId < clusters.size(); clusterId++)
        {

            // 1. Get your boundary loop (ordered indices)
            // Ensure this function returns the loop in consistent winding (CCW)
            const std::vector<size_t>& boundaryLoop = borders.at(clusterId);
            std::set<size_t>           boundarySet(boundaryLoop.begin(), boundaryLoop.end());
            const auto                 cluster = clusters.at(clusterId);
            std::set<size_t>           clusterVertecies;

            for (size_t triIdx : cluster)
            {
                clusterVertecies.insert(mesh.indices[3 * triIdx + 0]);
                clusterVertecies.insert(mesh.indices[3 * triIdx + 1]);
                clusterVertecies.insert(mesh.indices[3 * triIdx + 2]);
            }

            BRepBuilderAPI_MakeWire wireMaker;

            for (size_t i = 0; i < boundaryLoop.size(); ++i)
            {
                size_t idx1 = boundaryLoop[i];
                size_t idx2 = boundaryLoop[(i + 1) % boundaryLoop.size()];

                // Create a sorted key for the cache to find the undirected edge
                auto edgeKey = std::make_pair(std::min(idx1, idx2), std::max(idx1, idx2));

                TopoDS_Edge currentEdge;
                if (ecache.find(edgeKey) == ecache.end())
                {
                    // Not in cache: Create it
                    // Ensure vertices are in vcache
                    for (size_t vIdx : { idx1, idx2 })
                    {
                        if (vcache.find(vIdx) == vcache.end())
                        {
                            const auto& v = mesh.vertices[vIdx];
                            vcache[vIdx]  = gp_Pnt(v.x, v.y, v.z);
                        }
                    }
                    currentEdge     = BRepBuilderAPI_MakeEdge(vcache[idx1], vcache[idx2]);
                    ecache[edgeKey] = currentEdge;
                }
                else
                {
                    currentEdge = ecache[edgeKey];
                }

                // 2. Handle Orientation
                // The edge in the cache has a fixed direction (idx1_min -> idx1_max).
                // We must ensure it fits our loop's current flow.
                TopExp_Explorer exp(currentEdge, TopAbs_VERTEX);
                gp_Pnt          firstV = BRep_Tool::Pnt(TopoDS::Vertex(exp.Current()));

                // If the first vertex of the cached edge is not our current start point, reverse it
                if (!firstV.IsEqual(vcache[idx1], 1e-7))
                {
                    wireMaker.Add(TopoDS::Edge(currentEdge.Reversed()));
                }
                else
                {
                    wireMaker.Add(currentEdge);
                }
            }

            // 3. Setup the Plate Surface (NURBS reconstruction)
            BRepOffsetAPI_MakeFilling filler;
            if (wireMaker.IsDone())
            {
                TopoDS_Wire boundaryWire = wireMaker.Wire();

                // Iterate through the edges of the wire and add them individually
                for (TopExp_Explorer exp(boundaryWire, TopAbs_EDGE); exp.More(); exp.Next())
                {
                    const TopoDS_Edge& edge = TopoDS::Edge(exp.Current());
                    // Use the first overload from your header:
                    // Add(Edge, Continuity, IsBound)
                    filler.Add(edge, GeomAbs_C0, Standard_True);
                }
            }

            // 4. Add Internal Point Constraints
            for (size_t vIdx : clusterVertecies)
            {
                if (boundarySet.find(vIdx) == boundarySet.end())
                {
                    // 3. Ensure the point is in the cache
                    if (vcache.find(vIdx) == vcache.end())
                    {
                        const auto& v = mesh.vertices[vIdx];
                        vcache[vIdx]  = gp_Pnt(v.x, v.y, v.z);
                    }

                    // 4. Add the point constraint
                    filler.Add(vcache[vIdx]);
                }
            }

            // 5. Build and Post-Process
            filler.Build();
            if (filler.IsDone())
            {
                TopoDS_Face face = TopoDS::Face(filler.Shape());

                // Required for Sewing/Solid validity
                BRepLib::BuildCurves3d(face);
                resultFaces.push_back(face);
            }
        }

        return resultFaces;
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"
#include <BRepCheck_Analyzer.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>

using namespace Library;

TEST_CASE("Clusters2BREP - Single Quad Test", "[mesh2cad]")
{
    // 1. Setup a simple mesh (Two triangles forming a square in XY plane)
    // Vertices:
    // 0: (0,0,0), 1: (1,0,0), 2: (1,1,0), 3: (0,1,0)
    Triangulation mesh;
    mesh.vertices = {
        { 0, 0, 0 },  
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 0, 1, 0 }
    };
    // Triangles: (0,1,2) and (0,2,3)
    mesh.indices = { 0, 1, 2, 0, 2, 3 };

    // 2. Define one cluster containing both triangles
    std::vector<std::vector<size_t>> clusters = {
        { 0, 1 }
    };

    // 3. Define the border loop for the cluster: 0 -> 1 -> 2 -> 3
    std::vector<std::vector<size_t>> borders = {
        { 0, 1, 2, 3 }
    };

    // 4. Caches
    std::map<size_t, gp_Pnt>                         vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    // 5. Execute
    auto faces = mesh2cad_cluster::Clusters2BREP(clusters, borders, mesh, vcache, ecache);

    SECTION("Output Validity")
    {
        REQUIRE(faces.size() == 1);

        BRepCheck_Analyzer analyzer(faces[0]);
        CHECK(analyzer.IsValid() == Standard_True);
        CHECK_FALSE(faces[0].IsNull());
    }

    SECTION("Cache Population")
    {
        // vcache should contain all 4 vertices
        CHECK(vcache.size() == 4);
        CHECK(vcache.count(0) == 1);
        CHECK(vcache[1].X() == Approx(1.0));

        // ecache should contain the 4 boundary edges
        // (0,1), (1,2), (2,3), (0,3)
        CHECK(ecache.size() >= 4);
    }

    SECTION("Geometric Consistency")
    {
        // Check if the area is roughly correct (1.0 for a unit square)
        // This confirms the NURBS surface isn't wild
        GProp_GProps props;
        BRepGProp::SurfaceProperties(faces[0], props);
        CHECK(props.Mass() == Approx(1.0).margin(0.01));
    }
}

TEST_CASE("Clusters2BREP - Shared Edge Caching", "[mesh2cad]")
{
    Triangulation mesh;
    mesh.vertices = {
        {  0, 0, 0 },
        {  1, 0, 0 },
        {  1, 1, 0 },
        { -1, 0, 0 },
        { -1, 1, 0 }
    };
    mesh.indices = { 0, 1, 2, 0, 2, 4, 0, 4, 3 }; // Two distinct patches

    // Two clusters sharing edge (0, 2) or (0, 4) etc.
    std::vector<std::vector<size_t>> clusters = {
        { 0 },
        { 1, 2 }
    };
    std::vector<std::vector<size_t>> borders = {
        { 0, 1, 2 },
        { 0, 2, 4, 3 }
    };

    std::map<size_t, gp_Pnt>                         vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    auto faces = mesh2cad_cluster::Clusters2BREP(clusters, borders, mesh, vcache, ecache);

    SECTION("Topological Sharing")
    {
        REQUIRE(faces.size() == 2);

        // Find the shared edge (0,2) in both faces
        // If caching works, they should point to the exact same TShape
        auto findEdges = [](const TopoDS_Face& f)
        {
            std::vector<TopoDS_Edge> edges;
            for (TopExp_Explorer exp(f, TopAbs_EDGE); exp.More(); exp.Next())
            {
                edges.push_back(TopoDS::Edge(exp.Current()));
            }
            return edges;
        };

        auto edges1 = findEdges(faces[0]);
        auto edges2 = findEdges(faces[1]);

        bool foundShared = false;
        for (auto& e1 : edges1)
        {
            for (auto& e2 : edges2)
            {
                if (e1.IsSame(e2))
                    foundShared = true;
            }
        }
        CHECK(foundShared); // This proves ecache is actually deduplicating
    }
}
#endif