#include "mesh2cad_cluster.h"

#include "CAD/CADShape.h"
#include "CAD/CADShapeFactory.h"
#include "Clustering.h"
#include "HalfEdge/HalfEdge.h"
#include "HalfEdge/mesh2halfedge.h"
#include "Library/CAD/CADShapeFactory.h"
#include "Triangulation.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_MakeFilling.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
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

        // 1. Convert Borders of the Clusters to Wires
        std::map<size_t, TopoDS_Vertex>                  vcache;
        std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;
        std::vector<TopoDS_Wire>                         wires = Borders2Wires(borders, mesh, vcache, ecache);

        // 2. Convert Mesh Clusters to B-Rep Faces
        std::vector<TopoDS_Face> faces = Cluster2Faces(wires, cluster, mesh, vcache);

        // 3. Stitching Faces into a Shell
        TopoDS_Shape stich = StitchFaces(faces);

        // 4. Creating the Solid
        auto solid = MakeSolid(stich);

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
    std::vector<TopoDS_Face> mesh2cad_cluster::Cluster2Faces(const std::vector<TopoDS_Wire>&         wires,
                                                             const std::vector<std::vector<size_t>>& clusters,
                                                             const Triangulation&                    mesh,
                                                             std::map<size_t, TopoDS_Vertex>&        vcache)
    {
        std::vector<TopoDS_Face> faces;

        for (size_t i = 0; i < wires.size() && i < clusters.size(); ++i)
        {
            const auto&               boundaryWire = wires[i];
            BRepOffsetAPI_MakeFilling filler;

            // 1. Map vertices belonging to the wire to avoid redundant point constraints
            TopTools_IndexedMapOfShape wireVertices;
            TopExp::MapShapes(boundaryWire, TopAbs_VERTEX, wireVertices);

            // 2. Add Boundary Constraints
            for (TopExp_Explorer exp(boundaryWire, TopAbs_EDGE); exp.More(); exp.Next())
            {
                filler.Add(TopoDS::Edge(exp.Current()), GeomAbs_C0);
            }

            // 3. Add ONLY Internal Point Constraints
            for (size_t vIdx : clusters[i])
            {
                if (vcache.find(vIdx) == vcache.end())
                {
                    const auto& v = mesh.vertices[vIdx];
                    vcache[vIdx]  = BRepBuilderAPI_MakeVertex(gp_Pnt(v.x, v.y, v.z));
                }

                // Only add as a point constraint if it's NOT already part of the wire
                if (!wireVertices.Contains(vcache[vIdx]))
                {
                    filler.Add(BRep_Tool::Pnt(vcache[vIdx]));
                }
            }

            filler.Build();

            if (filler.IsDone())
            {
                TopoDS_Face F = TopoDS::Face(filler.Shape());
                BRepLib::BuildCurves3d(F);
                faces.push_back(F);
            }
        }
        return faces;
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

TEST_CASE("mesh2cad_cluster::Cluster2Faces - Surface Generation", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Define a 3D "tent" shape (a pyramid-like cluster)
    // 0(0,2,0) -- 1(2,2,0)
    //  |   4(1,1,1)   |
    // 3(0,0,0) -- 2(2,0,0)
    mesh.vertices = {
        { 0, 2, 0 },
        { 2, 2, 0 },
        { 2, 0, 0 },
        { 0, 0, 0 }, // Boundary
        { 1, 1, 1 }  // Internal peak
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    // 1. Setup the input: 1 Cluster with its boundary wire
    std::vector<std::vector<size_t>> clusters = {
        { 0, 1, 2, 3, 4 }
    };
    std::vector<std::vector<size_t>> borders = {
        { 0, 1, 2, 3 }
    };

    auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);
    REQUIRE(wires.size() == 1);

    // 2. Generate the Face
    auto faces = converter.Cluster2Faces(wires, clusters, mesh, vcache);

    // Test 1: Successful creation
    REQUIRE(faces.size() == 1);
    const auto& face = faces[0];
    CHECK(!face.IsNull());

    // Test 2: Topological Validity
    BRepCheck_Analyzer analyzer(face);
    CHECK(analyzer.IsValid() == Standard_True);

    // Test 3: Physical Properties (Area check)
    GProp_GProps props;
    BRepGProp::SurfaceProperties(face, props);
    double area = props.Mass();

    // A flat 2x2 square is area 4.0.
    // Our 'tent' peak at (1,1,1) should result in an area > 4.0.
    CHECK(area > 4.0);

    // Test 4: Boundary Integrity
    // Ensure the face's underlying surface is actually bounded by the wire
    TopExp_Explorer exp(face, TopAbs_WIRE);
    REQUIRE(exp.More());
    TopoDS_Wire extractedWire = TopoDS::Wire(exp.Current());
    CHECK(extractedWire.Closed() == Standard_True);

    // Test 5: Cache Persistence
    // The peak vertex (index 4) should now be in the vcache
    CHECK(vcache.count(4) == 1);
}

TEST_CASE("mesh2cad_cluster::Cluster2Faces - Planar Case", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Flat square on XY plane
    mesh.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 0, 1, 0 }
    };
    std::vector<std::vector<size_t>> clusters = {
        { 0, 1, 2, 3 }
    };
    std::vector<std::vector<size_t>> borders = {
        { 0, 1, 2, 3 }
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);
    auto faces = converter.Cluster2Faces(wires, clusters, mesh, vcache);

    REQUIRE(faces.size() == 1);

    GProp_GProps props;
    BRepGProp::SurfaceProperties(faces[0], props);
    // Area of 1x1 square should be approx 1.0
    CHECK(props.Mass() == Approx(1.0).margin(0.01));
}

TEST_CASE("mesh2cad_cluster::StitchFaces - Manifold Connectivity", "[mesh2cad]")
{
    Library::mesh2cad_cluster converter;
    Library::Triangulation    mesh;

    // Create a 2x1 grid (two adjacent squares)
    // 0---1---2
    // | A | B |
    // 3---4---5
    mesh.vertices = {
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 2, 1, 0 },
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 2, 0, 0 }
    };

    std::map<size_t, TopoDS_Vertex>                  vcache;
    std::map<std::pair<size_t, size_t>, TopoDS_Edge> ecache;

    // Define two adjacent clusters
    std::vector<std::vector<size_t>> clusters = {
        { 0, 1, 4, 3 }, // Square A
        { 1, 2, 5, 4 }  // Square B
    };

    // Define their boundary borders
    std::vector<std::vector<size_t>> borders = {
        { 0, 1, 4, 3 },
        { 1, 2, 5, 4 }
    };

    // 1. Generate Wires (using our cached shared edges)
    auto wires = converter.Borders2Wires(borders, mesh, vcache, ecache);
    REQUIRE(wires.size() == 2);

    // 2. Generate Faces
    auto faces = converter.Cluster2Faces(wires, clusters, mesh, vcache);
    REQUIRE(faces.size() == 2);

    // 3. Stitch the Faces
    // We use a slightly generous tolerance to account for NURBS approximation
    TopoDS_Shape stitched = converter.StitchFaces(faces, 1e-4);

    // Test 1: Shape Type
    // If the sewing is successful and the faces are connected,
    // the result should be a Shell, or a Compound containing exactly one Shell.
    CHECK((stitched.ShapeType() == TopAbs_SHELL || stitched.ShapeType() == TopAbs_COMPOUND));

    // Test 2: Shell Counting
    int shellCount = 0;
    for (TopExp_Explorer exp(stitched, TopAbs_SHELL); exp.More(); exp.Next())
    {
        shellCount++;
    }
    CHECK(shellCount == 1); // Both faces should belong to exactly one shell

    TopTools_IndexedMapOfShape edgeMap;
    TopExp::MapShapes(stitched, TopAbs_EDGE, edgeMap);

    // This will now correctly return 7
    CHECK(edgeMap.Extent() == 7);

    // Test 4: Validity
    BRepCheck_Analyzer analyzer(stitched);
    CHECK(analyzer.IsValid() == Standard_True);
}
#endif