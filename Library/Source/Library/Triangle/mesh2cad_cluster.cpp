#include "mesh2cad_cluster.h"

#include "CAD/CADShape.h"
#include "CAD/CADShapeFactory.h"
#include "Clustering.h"
#include "HalfEdge/HalfEdge.h"
#include "HalfEdge/HalfEdgeHealth.h"
#include "HalfEdge/mesh2halfedge.h"
#include "Library/CAD/CADShapeFactory.h"
#include "Triangulation.h"
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFill_Filling.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs_ShapeEnum.hxx> // For TopAbs_SHELL, etc.
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx> // For TopExp_Explorer
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS.hxx> // For the TopoDS casting utility
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <algorithm>
#include <gp_Pnt.hxx>
#include <map>
#include <set>
#include <vector>
#include <format>

namespace Library
{
    mesh2cad_cluster::mesh2cad_cluster() {}
    mesh2cad_cluster::~mesh2cad_cluster() {}

    std::unique_ptr<CADShape> mesh2cad_cluster::convert(const Triangulation& mesh, double threshold)
    {
        return convert(mesh,
                       [threshold](size_t currentIndex, size_t candidateIndex, const Triangulation& tri) -> bool
                       {
                           auto curr = tri.getFaceNormal(currentIndex);
                           auto cand = tri.getFaceNormal(candidateIndex);
                           return glm::dot(curr, cand) > threshold;
                       });
    }

    Triangulation extractTriangles(const Triangulation& source, const std::vector<size_t>& triangleIndicesToKeep)
    {
        Triangulation result;

        // Maps old vertex index -> new vertex index
        std::unordered_map<size_t, size_t> indexMap;

        for (size_t triIdx : triangleIndicesToKeep)
        {
            // Each triangle has 3 indices starting at triIdx * 3
            size_t startOffset = triIdx * 3;

            for (size_t i = 0; i < 3; ++i)
            {
                size_t oldVertexIdx = source.indices[startOffset + i];

                // Check if we've already copied this vertex
                auto it = indexMap.find(oldVertexIdx);
                if (it == indexMap.end())
                {
                    // New vertex found: add to result.vertices
                    size_t newIdx = result.vertices.size();
                    result.vertices.push_back(source.vertices[oldVertexIdx]);

                    // Map the old index to the new position
                    indexMap[oldVertexIdx] = newIdx;
                    result.indices.push_back(newIdx);
                }
                else
                {
                    // Vertex already exists: just use the mapped index
                    result.indices.push_back(it->second);
                }
            }
        }

        return result;
    }

    std::unique_ptr<CADShape> mesh2cad_cluster::convert(const Triangulation& mesh, std::function<bool(size_t currentIndex, size_t candidateIndex, const Triangulation&)> growFunction)
    {
        report       = "";
        bool healthy = Library::HalfEdgeHealth::isHealthy(mesh);
        if (!healthy)
        {
            report += "Unhealthy\n";
            return nullptr;
        }

        report += "MESH\n";
        report += HalfEdgeHealth::toString(mesh);
        report += "MESHEND\n";

        // 0. Preperation
        report += "Clustering\n";
        std::vector<std::vector<size_t>> cluster = Clustering::cluster_withoutHoles(mesh, growFunction);

        size_t counter = 0;
        for (const auto& c : cluster)
        {
            report += "  Cluster " + std::to_string(counter) + "\n ";

            for (const auto& x : c)
            {
                report += "    ";
                report += std::to_string(x);
                report += "\n";
            }

            auto triCluster = extractTriangles(mesh, c);
            report += triCluster.toBase64() + "\n";

            counter++;
        }

        report += "HalfEdge\n";
        auto                             halfedge = mesh2halfedge::convert(mesh);
        std::vector<std::vector<size_t>> borders;

        report += "Borders\n";
        counter = 0;
        for (const auto& c : cluster)
        {
            report += "  Border " + std::to_string(counter) + "\n";
            auto b = Clustering::findBorders(*halfedge, c)[0];
            borders.push_back(b); // withoutHoles ensures .size()==1
            for (size_t x : b)
            {
                report += "    ";
                report += std::to_string(x);
                report += "\n";
            }
            counter++;
        }

        // 1. Convert Borders of the Clusters to Wires
        std::map<size_t, TopoDS_Vertex>                  vcache;
        std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

        report += "Border2Wire\n";
        std::vector<TopoDS_Wire> wires = Borders2Wires(borders, mesh, vcache, ecache);

        // 2. Convert Mesh Clusters to B-Rep Faces
        report += "Cluster2Faces\n";
        std::vector<TopoDS_Face> faces = Cluster2Faces(wires, cluster, mesh, vcache);
        if (faces.size() == 0)
        {
            std::ofstream out("C:\\Users\\nicol\\Downloads\\crashlog.txt");
            out << report;
            out.close();
            return nullptr;
        }

        // 3. Stitching Faces into a Shell
        TopoDS_Shape stich = StitchFaces(faces);

        // 4. Creating the Solid
        auto solid = MakeSolid(stich);
        BRepLib::OrientClosedSolid(solid);

        auto result = CADShapeFactory::make(solid);
        CADShapeFactory::recurseFillChildShapes(*result);
        return std::move(result);
    }

    TopoDS_Solid mesh2cad_cluster::MakeSolid(const TopoDS_Shape& stitchedShell)
    {
        // 1. Safety check: Ensure we are dealing with a shell or a compound of shells
        if (stitchedShell.IsNull())
            return TopoDS_Solid();

        // 2. Use the MakeSolid tool
        // If stitchedShell is a TopoDS_Shell, this wraps it.
        // If it's a Compound, it tries to extract shells to form a solid.
        BRepBuilderAPI_MakeSolid solidMaker;

        for (TopExp_Explorer exp(stitchedShell, TopAbs_SHELL); exp.More(); exp.Next())
        {
            solidMaker.Add(TopoDS::Shell(exp.Current()));
        }

        if (!solidMaker.IsDone())
        {
            // This usually happens if the shell is not closed (has free edges)
            return TopoDS_Solid();
        }

        return solidMaker.Solid();
    }

    std::vector<TopoDS_Wire> mesh2cad_cluster::Borders2Wires(const std::vector<std::vector<size_t>>&           borders,
                                                             const Triangulation&                              mesh,
                                                             std::map<size_t, TopoDS_Vertex>&                  vcache, // Changed from gp_Pnt to TopoDS_Vertex
                                                             std::map<std::pair<size_t, size_t>, TopoDS_Edge>& ecache)
    {
        std::vector<TopoDS_Wire> resultWires;

        for (const auto& border : borders)
        {
            BRepBuilderAPI_MakeWire wireMaker;

            for (size_t i = 0; i < border.size(); ++i)
            {
                size_t idxA = border[i];
                size_t idxB = border[(i + 1) % border.size()];

                // 1. Get/Create shared Vertices
                auto getVtx = [&](size_t idx)
                {
                    if (vcache.find(idx) == vcache.end())
                    {
                        const auto& v = mesh.vertices[idx];
                        vcache[idx]   = BRepBuilderAPI_MakeVertex(gp_Pnt(v.x, v.y, v.z));
                    }
                    return vcache[idx];
                };

                TopoDS_Vertex vA = getVtx(idxA);
                TopoDS_Vertex vB = getVtx(idxB);

                // 2. Normalize key
                auto edgeKey = (idxA < idxB) ? std::make_pair(idxA, idxB) : std::make_pair(idxB, idxA);

                // 3. Get/Create Edge using shared Vertices
                TopoDS_Edge edge;
                if (ecache.find(edgeKey) == ecache.end())
                {
                    edge            = BRepBuilderAPI_MakeEdge(vA, vB);
                    ecache[edgeKey] = edge;
                }
                else
                {
                    edge = ecache[edgeKey];
                }

                wireMaker.Add(edge);
            }

            if (wireMaker.IsDone())
                resultWires.push_back(wireMaker.Wire());
        }
        return resultWires;
    }
    std::string AnalyzeFillingInput(int index, const TopoDS_Wire& wire, const std::vector<size_t>& clusterIndices, std::map<size_t, TopoDS_Vertex>& vcache)
    {
        std::stringstream ss;
        ss << "\n=== [Face " << index << " Pre-Flight Check] ===\n";

        // 1. Wire Topology Check
        if (wire.IsNull())
        {
            ss << "FAILURE: Wire is null.\n";
        }
        else
        {
            BRepCheck_Analyzer analyzer(wire);
            ss << "Wire Valid: " << (analyzer.IsValid() ? "YES" : "NO") << "\n";
            ss << "Wire Closed: " << (BRep_Tool::IsClosed(wire) ? "YES" : "NO") << "\n";

            int edgeCount = 0;
            for (TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next())
                edgeCount++;
            ss << "Edge Count: " << edgeCount << "\n";
        }

        // 2. Vertex/Point Analysis
        TopTools_IndexedMapOfShape wireVertices;
        if (!wire.IsNull())
        {
            TopExp::MapShapes(wire, TopAbs_VERTEX, wireVertices);
        }

        ss << "Total Indices in Cluster: " << clusterIndices.size() << "\n";
        ss << "Vertices in Wire: " << wireVertices.Extent() << "\n";

        int redundantPoints = 0;
        for (size_t vIdx : clusterIndices)
        {
            if (vcache.count(vIdx) && wireVertices.Contains(vcache[vIdx]))
            {
                redundantPoints++;
            }
        }
        ss << "Redundant Points (on boundary): " << redundantPoints << "\n";
        ss << "Internal Constraints (points): " << (clusterIndices.size() - redundantPoints) << "\n";

        ss << "==========================================\n";
        return ss.str();
    }

    std::vector<TopoDS_Face> mesh2cad_cluster::Cluster2Faces(const std::vector<TopoDS_Wire>&         wires,
                                                             const std::vector<std::vector<size_t>>& clusters,
                                                             const Triangulation&                    mesh,
                                                             std::map<size_t, TopoDS_Vertex>&        vcache)
    {
        std::vector<TopoDS_Face> result_faces;
        size_t                   count = std::min(wires.size(), clusters.size());

        for (size_t i = 0; i < count; ++i)
        {
            report += "Cluster2Faces::Cluster: " + std::to_string(i) + "\n";

            const TopoDS_Wire& current_wire = wires[i];
            if (current_wire.IsNull())
                continue;

            try
            {
                BRepFill_Filling filler;

                // 1. Add Wire Edges as Boundary Constraints
                for (TopoDS_Iterator it(current_wire); it.More(); it.Next())
                {
                    TopoDS_Edge edge = TopoDS::Edge(it.Value());
                    if (!edge.IsNull())
                    {
                        filler.Add(edge, GeomAbs_C0);
                    }
                }

                // fill in some sample points
                const auto& tri_indices  = clusters[i];
                size_t      sample_count = 15;
                size_t      stride       = std::max((size_t)1, tri_indices.size() / sample_count);
                for (size_t j = 0; j < tri_indices.size(); j += stride)
                {
                    size_t tri_idx = tri_indices[j];

                    // Calculate the Centroid (Center of the triangle)
                    size_t v1_idx = mesh.indices[tri_idx * 3 + 0];
                    size_t v2_idx = mesh.indices[tri_idx * 3 + 1];
                    size_t v3_idx = mesh.indices[tri_idx * 3 + 2];

                    const auto& v1 = mesh.vertices[v1_idx];
                    const auto& v2 = mesh.vertices[v2_idx];
                    const auto& v3 = mesh.vertices[v3_idx];

                    gp_Pnt centroid((v1.x + v2.x + v3.x) / 3.0, (v1.y + v2.y + v3.y) / 3.0, (v1.z + v2.z + v3.z) / 3.0);

                    // Add the centroid as an internal constraint
                    filler.Add(centroid);
                }

                // 3. Build the Face
                filler.Build();

                if (filler.IsDone())
                {
                    TopoDS_Face skinned_face = filler.Face();
                    if (!skinned_face.IsNull())
                    {
                        result_faces.push_back(skinned_face);
                    }
                    else
                    {

                        return {};
                    }
                }
                else
                {
                    return {};
                }
            }
            catch (Standard_Failure const&)
            {
                // Skip problematic clusters to maintain pipeline flow
                continue;
            }
        }

        return result_faces;
    }

    TopoDS_Shape mesh2cad_cluster::StitchFaces(const std::vector<TopoDS_Face>& faces, double tolerance)
    {
        // 1. Initialize the Sewing tool
        // The tolerance is the maximum gap it will attempt to bridge
        BRepBuilderAPI_Sewing sewer(tolerance);

        // 2. Add all generated faces to the sewer
        for (const auto& face : faces)
        {
            sewer.Add(face);
        }

        // 3. Perform the sewing operation
        sewer.Perform();

        // 4. Retrieve the resulting shape
        // This usually returns a TopoDS_Shell if successful,
        // or a TopoDS_Compound if the faces couldn't form a single manifold shell.
        TopoDS_Shape result = sewer.SewedShape();

        return result;
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <GProp_GProps.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

using namespace Library;

Triangulation CreateUnitCubeMesh()
{
    Triangulation m;
    m.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 1, 1, 1 },
        { 0, 1, 1 }
    };
    m.indices = { 0, 3, 2, 0, 2, 1, 4, 5, 6, 4, 6, 7, 0, 1, 5, 0, 5, 4, 1, 2, 6, 1, 6, 5, 2, 3, 7, 2, 7, 6, 3, 0, 4, 3, 4, 7 };
    return m;
}

TEST_CASE("mesh2cad_cluster::Borders2Wires - Basic Loops", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Define a simple quad (2 triangles)
    // 0(0,1) -- 1(1,1)
    //  |         |
    // 3(0,0) -- 2(1,0)
    mesh.vertices = {
        { 0.0, 1.0, 0.0 },
        { 1.0, 1.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0 }
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    SECTION("Single closed triangular border")
    {
        std::vector<std::vector<size_t>> borders = {
            { 0, 1, 2 }
        };
        auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);

        REQUIRE(wires.size() == 1);
        const auto& wire = wires[0];

        // 1. Check validity via OpenCASCADE Analyzer
        BRepCheck_Analyzer analyzer(wire);
        CHECK(analyzer.IsValid() == Standard_True);

        // 2. Check if it's closed
        CHECK(wire.Closed() == Standard_True);

        // 3. Count edges (should be 3)
        int edgeCount = 0;
        for (TopExp_Explorer exp(wire, TopAbs_EDGE); exp.More(); exp.Next())
        {
            edgeCount++;
        }
        CHECK(edgeCount == 3);
    }

    SECTION("Caching mechanism - Shared edges")
    {
        // Two adjacent loops sharing edge 1-2
        std::vector<std::vector<size_t>> borders = {
            { 0, 1, 2 },
            { 2, 1, 3 }
        };

        auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);

        REQUIRE(wires.size() == 2);

        // Verify ecache contains the shared edge (1,2)
        // Normalized key for {1,2} and {2,1} is the same
        std::pair<size_t, size_t> sharedKey = { 1, 2 };
        CHECK(ecache.count(sharedKey) > 0);

        // Ensure total unique edges in cache is 5 (3 from first tri + 2 new ones from second)
        CHECK(ecache.size() == 5);
    }

    SECTION("Geometric Accuracy")
    {
        std::vector<std::vector<size_t>> borders = {
            { 0, 1, 2, 3 }
        };
        auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);

        TopExp_Explorer exp(wires[0], TopAbs_EDGE);
        TopoDS_Edge     firstEdge = TopoDS::Edge(exp.Current());

        // Extract vertices from the first edge
        TopoDS_Vertex vFirst, vLast;
        TopExp::Vertices(firstEdge, vFirst, vLast);

        gp_Pnt p = BRep_Tool::Pnt(vFirst);
        // Expecting point 0 (0,1,0)
        CHECK(p.X() == Approx(0.0));
        CHECK(p.Y() == Approx(1.0));
        CHECK(p.Z() == Approx(0.0));
    }
}

TEST_CASE("mesh2cad_cluster::Borders2Wires - Topology Sharing", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Define vertices for two adjacent triangles
    // 0 --- 1 --- 4
    //  \   / \   /
    //    2 --- 3
    mesh.vertices = {
        { 0, 2, 0 },
        { 2, 2, 0 },
        { 1, 0, 0 }, // Tri 0: 0, 1, 2
        { 3, 0, 0 },
        { 4, 2, 0 }  // Tri 1: 1, 4, 3, 2 (Quad/Tri overlap)
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    // We define two borders that share the edge between vertex 1 and vertex 2
    std::vector<size_t> borderA = { 0, 1, 2 };
    std::vector<size_t> borderB = { 1, 4, 3, 2 }; // Shared segment is 1-2

    std::vector<std::vector<size_t>> allBorders = { borderA, borderB };
    auto                             wires      = converter.Borders2Wires(allBorders, mesh, vcache, ecache);

    REQUIRE(wires.size() == 2);

    // 1. Verify ecache logic: The key (1, 2) must exist
    std::pair<size_t, size_t> sharedKey = { 1, 2 }; // (smaller index first)
    REQUIRE(ecache.count(sharedKey) == 1);
    TopoDS_Edge cachedSharedEdge = ecache[sharedKey];

    // Get the edge from Wire A
    TopExp_Explorer expA(wires[0], TopAbs_EDGE);
    expA.Next(); // Move to the second edge (1-2)
    TopoDS_Edge edgeFromWireA = TopoDS::Edge(expA.Current());

    // Get the edge from Wire B
    TopExp_Explorer expB(wires[1], TopAbs_EDGE);
    expB.Next();
    expB.Next();
    expB.Next(); // Move to the fourth edge (2-1)
    TopoDS_Edge edgeFromWireB = TopoDS::Edge(expB.Current());

    // 1. Check if they share the same TShape (The absolute truth in OCC)
    bool tShapeMatch = (edgeFromWireA.TShape() == edgeFromWireB.TShape());
    CHECK(tShapeMatch);

    // 2. Check if either matches the cache
    bool cacheMatch = (edgeFromWireA.TShape() == cachedSharedEdge.TShape());
    CHECK(cacheMatch);

    // 2. Verify Wire A contains the cached edge
    bool wireAHasSharedEdge = false;
    for (TopExp_Explorer exp(wires[0], TopAbs_EDGE); exp.More(); exp.Next())
    {
        if (exp.Current().IsPartner(cachedSharedEdge))
        {
            wireAHasSharedEdge = true;
            break;
        }
    }
    CHECK(wireAHasSharedEdge);

    // 3. Verify Wire B contains the EXACT SAME cached edge
    bool wireBHasSharedEdge = false;
    for (TopExp_Explorer exp(wires[1], TopAbs_EDGE); exp.More(); exp.Next())
    {
        // IsSame checks if they point to the same geometric/topological definition
        if (exp.Current().IsPartner(cachedSharedEdge))
        {
            wireBHasSharedEdge = true;
            break;
        }
    }
    CHECK(wireBHasSharedEdge);

    // 4. Final check: Total edges created should be 6
    // (Tri A: 3 edges) + (Tri B: 4 edges) - (1 shared edge) = 6
    CHECK(ecache.size() == 6);
}

TEST_CASE("mesh2cad_cluster::convert - Full Cube Reconstruction", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh = CreateUnitCubeMesh();

    // 3. Run the full conversion
    // Threshold 0.9 ensures triangles with the same normal cluster together
    double                             threshold = 0.9;
    std::unique_ptr<Library::CADShape> result    = converter.convert(mesh, threshold);

    // --- VERIFICATION ---

    REQUIRE(result != nullptr);
    TopoDS_Shape shape = result->getData();
    REQUIRE(!shape.IsNull());

    // Test 1: Shape Identity
    // It should be a Solid
    CHECK(shape.ShapeType() == TopAbs_SOLID);

    // Test 2: Face Count
    // A cube has 6 faces. clustering 12 triangles by normals should yield 6 CAD faces.
    int faceCount = 0;
    for (TopExp_Explorer exp(shape, TopAbs_FACE); exp.More(); exp.Next())
    {
        faceCount++;
    }
    CHECK(faceCount == 6);

    // Test 3: Volume Accuracy
    // 1x1x1 cube should have a volume of exactly 1.0
    GProp_GProps vProps;
    BRepGProp::VolumeProperties(shape, vProps);
    CHECK(vProps.Mass() == Approx(1.0).margin(1e-4));

    // Test 4: Surface Area Accuracy
    // 6 faces of 1x1 = 6.0
    GProp_GProps sProps;
    BRepGProp::SurfaceProperties(shape, sProps);
    CHECK(sProps.Mass() == Approx(6.0).margin(1e-4));

    // Test 5: Topological Validity
    BRepCheck_Analyzer analyzer(shape);
    CHECK(analyzer.IsValid() == Standard_True);

    // Test 6: Watertightness
    // No free edges in a solid cube
    int freeEdges = 0;
    // (You could also use BRepBuilderAPI_Sewing internally to verify this)
}

#endif