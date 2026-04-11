#include "mesh2halfedge.h"

#include <iomanip>
#include <sstream>

namespace Library
{
    std::unique_ptr<HalfEdgeMesh> mesh2halfedge::convert(const Triangulation& input)
    {
        auto mesh = std::make_unique<HalfEdgeMesh>();

        // 1. Copy over the vertices
        mesh->vertices.reserve(input.vertices.size());
        for (const auto& p : input.vertices)
        {
            mesh->vertices.push_back({ p, SafeNull });
        }

        // 2. Prepare storage
        size_t num_triangles = input.indices.size() / 3;
        mesh->faces.reserve(num_triangles);
        mesh->half_edges.reserve(input.indices.size());

        // Map to track edges and find twins:
        // Key: pair of vertex indices (sorted smallest to largest)
        // Value: The index of the half-edge we've already created for this span
        std::map<std::pair<int64_t, int64_t>, int64_t> edge_map;

        for (size_t i = 0; i < input.indices.size(); i += 3)
        {
            int64_t f_idx = static_cast<int64_t>(mesh->faces.size());
            mesh->faces.push_back({ SafeNull });

            int64_t v_indices[3] = { static_cast<int64_t>(input.indices[i]), static_cast<int64_t>(input.indices[i + 1]), static_cast<int64_t>(input.indices[i + 2]) };

            int64_t e_indices[3];

            // Create the three half-edges for this face
            for (int j = 0; j < 3; ++j)
            {
                e_indices[j] = static_cast<int64_t>(mesh->half_edges.size());
                mesh->half_edges.push_back(HalfEdge());

                HalfEdge& e     = mesh->half_edges.back();
                e.target_vertex = v_indices[(j + 1) % 3];
                e.face          = f_idx;

                // Link vertex to one of its outgoing half-edges
                mesh->vertices[v_indices[j]].half_edge = e_indices[j];
            }

            // Set 'next' pointers for the face loop
            for (int j = 0; j < 3; ++j)
            {
                mesh->half_edges[e_indices[j]].next = e_indices[(j + 1) % 3];
            }

            // Set face's starting edge
            mesh->faces[f_idx].half_edge = e_indices[0];

            // --- Twin Linking ---
            for (int j = 0; j < 3; ++j)
            {
                int64_t v_start = v_indices[j];
                int64_t v_end   = v_indices[(j + 1) % 3];

                // Create a canonical key (smaller index first)
                auto key = std::make_pair(std::min(v_start, v_end), std::max(v_start, v_end));

                if (edge_map.count(key))
                {
                    // Twin found! Link them
                    int64_t existing_e                  = edge_map[key];
                    mesh->half_edges[e_indices[j]].twin = existing_e;
                    mesh->half_edges[existing_e].twin   = e_indices[j];
                    // Remove from map if you want to optimize for memory,
                    // or keep it to detect non-manifold edges.
                }
                else
                {
                    // First time seeing this edge
                    edge_map[key] = e_indices[j];
                }
            }
        }

        return mesh;
    }

    std::unique_ptr<Triangulation> mesh2halfedge::convert(const HalfEdgeMesh& input)
    {
        auto tri = std::make_unique<Triangulation>();

        // 1. Copy over positions
        tri->vertices.reserve(input.vertices.size());
        for (const auto& v : input.vertices)
        {
            tri->vertices.push_back(v.position);
        }

        // 2. Build the index buffer
        // Each face in a HalfEdgeMesh is a loop of half-edges.
        // For a triangulation, we assume each face has exactly 3 half-edges.
        tri->indices.reserve(input.faces.size() * 3);

        for (const auto& f : input.faces)
        {
            if (f.half_edge == SafeNull)
                continue;

            // Start at the anchor half-edge
            int64_t e0 = f.half_edge;
            int64_t e1 = input.half_edges[e0].next;
            int64_t e2 = input.half_edges[e1].next;

            // Grab the target vertex of each half-edge in the loop
            // Note: In a CCW loop, the target of e0 is the second vertex,
            // the target of e1 is the third, and the target of e2 returns to the first.
            tri->indices.push_back(static_cast<size_t>(input.half_edges[e2].target_vertex));
            tri->indices.push_back(static_cast<size_t>(input.half_edges[e0].target_vertex));
            tri->indices.push_back(static_cast<size_t>(input.half_edges[e1].target_vertex));
        }

        return tri;
    }

    std::string mesh2halfedge::toString(const HalfEdgeMesh& target)
    {
        std::stringstream ss;
        ss << "=== HalfEdgeMesh Dump ===\n";

        // 1. Dump Vertices
        ss << "Vertices [" << target.vertices.size() << "]:\n";
        for (size_t i = 0; i < target.vertices.size(); ++i)
        {
            ss << std::setw(4) << i << ": pos(" << target.vertices[i].position.x << ", " << target.vertices[i].position.y << ", " << target.vertices[i].position.z
               << ") | out_edge: " << target.vertices[i].half_edge << "\n";
        }

        // 2. Dump Half-Edges (The most important part)
        ss << "\nHalf-Edges [" << target.half_edges.size() << "]:\n";
        ss << "  ID | Target | Next | Twin | Face | Source (via Twin)\n";
        ss << "------------------------------------------------------\n";
        for (size_t i = 0; i < target.half_edges.size(); ++i)
        {
            const auto& e = target.half_edges[i];

            // Safety check for source calculation to avoid crashes during dump
            std::string src_str = (e.twin != SafeNull) ? std::to_string(target.source(i)) : "Boundary";

            ss << std::setw(4) << i << " | " << std::setw(6) << e.target_vertex << " | " << std::setw(4) << e.next << " | " << std::setw(4) << e.twin << " | " << std::setw(4) << e.face << " | "
               << src_str << "\n";
        }

        // 3. Dump Faces
        ss << "\nFaces [" << target.faces.size() << "]:\n";
        for (size_t i = 0; i < target.faces.size(); ++i)
        {
            ss << std::setw(4) << i << ": start_edge: " << target.faces[i].half_edge << "\n";
        }

        ss << "========================\n";
        return ss.str();
    }

    std::string mesh2halfedge::createReport(const HalfEdgeMesh& mesh)
    {
        std::stringstream        ss;
        size_t                   open_edges         = 0;
        size_t                   twin_mismatches    = 0;
        size_t                   invalid_next_loops = 0;
        size_t                   null_faces         = 0;
        size_t                   isolated_verts     = 0;
        std::vector<std::string> critical_logs;

        // 1. Audit Half-Edges
        for (size_t i = 0; i < mesh.half_edges.size(); ++i)
        {
            const auto& he = mesh.half_edges[i];

            // Check Twins
            if (he.twin == SafeNull)
            {
                open_edges++;
            }
            else if (he.twin < 0 || he.twin >= (int64_t)mesh.half_edges.size())
            {
                critical_logs.push_back("E" + std::to_string(i) + ": Twin index out of bounds (" + std::to_string(he.twin) + ")");
            }
            else if (mesh.half_edges[he.twin].twin != (int64_t)i)
            {
                twin_mismatches++;
                critical_logs.push_back("E" + std::to_string(i) + ": Twin mismatch. Twin's twin is " + std::to_string(mesh.half_edges[he.twin].twin));
            }

            // Check Face Loops (Topology)
            if (he.next == SafeNull)
            {
                critical_logs.push_back("E" + std::to_string(i) + ": Next pointer is SafeNull.");
            }
            else
            {
                // In a triangulation, following 'next' 3 times must return to start
                int64_t n1 = he.next;
                int64_t n2 = (n1 >= 0 && n1 < mesh.half_edges.size()) ? mesh.half_edges[n1].next : SafeNull;
                int64_t n3 = (n2 >= 0 && n2 < mesh.half_edges.size()) ? mesh.half_edges[n2].next : SafeNull;

                if (n3 != (int64_t)i)
                {
                    invalid_next_loops++;
                    critical_logs.push_back("E" + std::to_string(i) + ": Does not form a 3-edge loop (Triangle check failed).");
                }
            }
            // 1. TWIN SYMMETRY (Critical for your isHole assert)
            if (he.twin != SafeNull)
            {
                if (he.twin < 0 || he.twin >= (int64_t)mesh.half_edges.size())
                {
                    critical_logs.push_back("E" + std::to_string(i) + ": Twin index OOB (" + std::to_string(he.twin) + ")");
                }
                else if (mesh.half_edges[he.twin].twin != i)
                {
                    // This is the most likely reason your cluster assert fails
                    critical_logs.push_back("E" + std::to_string(i) + ": Twin mismatch. Twin's twin is " + std::to_string(mesh.half_edges[he.twin].twin));
                }
            }

            // 2. VERTEX CONTINUITY (Ensures loopEdges are actually a path)
            if (he.next != SafeNull && he.next < (int64_t)mesh.half_edges.size())
            {
                int64_t my_target   = he.target_vertex;
                int64_t next_source = mesh.source(he.next);
                if (my_target != next_source)
                {
                    critical_logs.push_back("E" + std::to_string(i) + ": Continuity break. Target V" + std::to_string(my_target) + " != Next Source V" + std::to_string(next_source));
                }
            }

            // 3. FACE BACK-LINK (Validates the face index used in clustering)
            if (he.face != SafeNull)
            {
                if (he.face < 0 || he.face >= (int64_t)mesh.faces.size())
                {
                    critical_logs.push_back("E" + std::to_string(i) + ": Invalid Face index " + std::to_string(he.face));
                }
                else if (mesh.faces[he.face].half_edge == SafeNull)
                {
                    critical_logs.push_back("E" + std::to_string(i) + ": Belongs to Face " + std::to_string(he.face) + " but Face has no root edge.");
                }
            }
        }

        // 2. Audit Vertices
        for (size_t i = 0; i < mesh.vertices.size(); ++i)
        {
            if (mesh.vertices[i].half_edge == SafeNull)
            {
                isolated_verts++;
            }
        }

        // 3. Audit Faces
        for (size_t i = 0; i < mesh.faces.size(); ++i)
        {
            if (mesh.faces[i].half_edge == SafeNull)
            {
                null_faces++;
            }
        }

        // 4. Formatting the Output
        ss << "=== Half-Edge Mesh Debug Report ===\n";
        ss << "General Stats:\n";
        ss << "  - Vertices:   " << mesh.vertices.size() << " (" << isolated_verts << " isolated)\n";
        ss << "  - Faces:      " << mesh.faces.size() << " (" << null_faces << " empty)\n";
        ss << "  - Half-Edges: " << mesh.half_edges.size() << "\n\n";

        ss << "Topological Integrity:\n";
        ss << "  - Boundary (Open) Edges: " << open_edges << "\n";
        ss << "  - Twin Mismatches:       " << twin_mismatches << "\n";
        ss << "  - Invalid Face Loops:    " << invalid_next_loops << "\n";
        ss << "  - Manifold Property:     " << ((open_edges == 0 && twin_mismatches == 0) ? "YES" : "NO") << "\n\n";

        if (!critical_logs.empty())
        {
            ss << "Critical Error Log (First 10):\n";
            for (size_t i = 0; i < critical_logs.size() && i < 10; ++i)
            {
                ss << "  [!] " << critical_logs[i] << "\n";
            }
            if (critical_logs.size() > 10)
                ss << "  ... and " << (critical_logs.size() - 10) << " more errors.\n";
        }
        else
        {
            ss << "Status: All connectivity checks passed.\n";
        }

        return ss.str();
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"
using namespace Library;
TEST_CASE("mesh2halfedge/Mesh to Half-Edge Conversion", "[mesh][converter]")
{

    // Setup: A simple Tetrahedron (4 vertices, 4 faces)
    Triangulation tri;
    tri.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 0, 0, 1 }
    };
    // 4 triangles (CCW winding)
    tri.indices = { 0, 1, 2, 0, 2, 3, 0, 3, 1, 1, 3, 2 };

    SECTION("Conversion to Half-Edge Mesh")
    {
        auto heMesh = mesh2halfedge::convert(tri);

        REQUIRE(heMesh != nullptr);
        CHECK(heMesh->vertices.size() == 4);
        CHECK(heMesh->faces.size() == 4);
        // Each triangle has 3 half-edges
        CHECK(heMesh->half_edges.size() == 12);

        SECTION("Topology Integrity")
        {
            for (size_t i = 0; i < heMesh->half_edges.size(); ++i)
            {
                const auto& edge = heMesh->half_edges[i];

                // 1. Every edge must have a valid twin in a closed manifold
                CHECK(edge.twin != SafeNull);

                // 2. Twin of a twin is the edge itself
                CHECK(heMesh->half_edges[edge.twin].twin == (int64_t)i);

                // 3. Next pointers must form a triangle (next->next->next == self)
                int64_t e1 = i;
                int64_t e2 = heMesh->next(e1);
                int64_t e3 = heMesh->next(e2);
                CHECK(heMesh->next(e3) == e1);

                // 4. Source of edge must be target of the previous edge
                // Finding previous is usually done via next->next in a triangle
                CHECK(heMesh->source(e1) == heMesh->half_edges[e3].target_vertex);
            }
        }
        SECTION("Geometric Consistency")
        {
            for (size_t i = 0; i < tri.vertices.size(); ++i)
            {
                auto const& hePos  = heMesh->vertices[i].position;
                auto const& triPos = tri.vertices[i];

                INFO("Vertex index: " << i);
                CHECK_THAT(hePos.x, Catch::Matchers::WithinAbs(triPos.x, 1e-9));
                CHECK_THAT(hePos.y, Catch::Matchers::WithinAbs(triPos.y, 1e-9));
                CHECK_THAT(hePos.z, Catch::Matchers::WithinAbs(triPos.z, 1e-9));
            }
        }
    }

    SECTION("Round-trip Conversion (Tri -> HE -> Tri)")
    {
        auto heMesh    = mesh2halfedge::convert(tri);
        auto backToTri = mesh2halfedge::convert(*heMesh);

        REQUIRE(backToTri->vertices.size() == tri.vertices.size());
        REQUIRE(backToTri->indices.size() == tri.indices.size());

        // Note: The order of indices might change depending on your implementation,
        // but the set of triangles should be the same.
    }
}

TEST_CASE("Single Triangle (Open Mesh) Test", "[mesh]")
{
    Triangulation tri;
    tri.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 }
    };
    tri.indices = { 0, 1, 2 };

    auto heMesh = mesh2halfedge::convert(tri);

    // In an open mesh, some half-edges might not have twins
    // unless your converter creates "boundary" half-edges.
    REQUIRE(heMesh->faces.size() == 1);
    CHECK(heMesh->half_edges.size() == 3);
}
bool areTriangulationsEqual(const Library::Triangulation& a, const Library::Triangulation& b)
{
    if (a.vertices.size() != b.vertices.size())
        return false;
    if (a.indices.size() != b.indices.size())
        return false;

    // Compare vertices with a small epsilon for doubles
    for (size_t i = 0; i < a.vertices.size(); ++i)
    {
        if (glm::distance(a.vertices[i], b.vertices[i]) > 1e-9)
            return false;
    }

    // Compare indices
    for (size_t i = 0; i < a.indices.size(); ++i)
    {
        if (a.indices[i] != b.indices[i])
            return false;
    }

    return true;
}

TEST_CASE("Converter Roundtrip: Tri -> HE -> Tri", "[mesh][roundtrip]")
{
    using namespace Library;

    // 1. Setup an initial Triangulation (a simple quad made of 2 triangles)
    Triangulation original;
    original.vertices = {
        { 0.0, 0.0, 0.0 },
        { 1.0, 0.0, 0.0 },
        { 1.0, 1.0, 0.0 },
        { 0.0, 1.0, 0.0 }
    };
    original.indices = {
        0, 1, 2, // Triangle 1
        0, 2, 3  // Triangle 2
    };

    // 2. First Conversion: Triangulation -> HalfEdgeMesh
    auto heMesh = mesh2halfedge::convert(original);
    REQUIRE(heMesh != nullptr);

    // Quick sanity check on the intermediate HE mesh
    CHECK(heMesh->vertices.size() == 4);
    CHECK(heMesh->faces.size() == 2);
    // 2 faces * 3 edges = 6 half-edges
    CHECK(heMesh->half_edges.size() == 6);

    // 3. Second Conversion: HalfEdgeMesh -> Triangulation
    auto resultTri = mesh2halfedge::convert(*heMesh);
    REQUIRE(resultTri != nullptr);

    // 4. Final Comparison
    SECTION("Topology and Geometry preserved")
    {
        CHECK(resultTri->vertices.size() == original.vertices.size());
        CHECK(resultTri->indices.size() == original.indices.size());

        for (size_t i = 0; i < original.vertices.size(); ++i)
        {
            INFO("Vertex index: " << i);
            CHECK_THAT(resultTri->vertices[i].x, Catch::Matchers::WithinAbs(original.vertices[i].x, 1e-9));
            CHECK_THAT(resultTri->vertices[i].y, Catch::Matchers::WithinAbs(original.vertices[i].y, 1e-9));
            CHECK_THAT(resultTri->vertices[i].z, Catch::Matchers::WithinAbs(original.vertices[i].z, 1e-9));
        }

        // Check if indices match (assumes converter preserves order)
        CHECK(resultTri->indices == original.indices);
    }
}
TEST_CASE("Converter Roundtrip: HE -> Tri -> HE", "[mesh][roundtrip]")
{
    using namespace Library;

    // Manually construct a single HE triangle
    auto originalHE = std::make_unique<HalfEdgeMesh>();
    originalHE->vertices.push_back({
      { 0, 0, 0 },
      0
    });
    originalHE->vertices.push_back({
      { 1, 0, 0 },
      1
    });
    originalHE->vertices.push_back({
      { 0, 1, 0 },
      2
    });

    // Construct 3 half-edges for one face
    // Edge 0: 0->1, Edge 1: 1->2, Edge 2: 2->0
    originalHE->half_edges.push_back({ 1, 1, SafeNull, 0 });
    originalHE->half_edges.push_back({ 2, 2, SafeNull, 0 });
    originalHE->half_edges.push_back({ 0, 0, SafeNull, 0 });
    originalHE->faces.push_back({ 0 });

    // 1. Convert HE -> Tri
    auto tri = mesh2halfedge::convert(*originalHE);
    REQUIRE(tri->indices.size() == 3);

    // 2. Convert Tri -> HE
    auto resultHE = mesh2halfedge::convert(*tri);

    // 3. Verify
    CHECK(resultHE->vertices.size() == originalHE->vertices.size());
    CHECK(resultHE->faces.size() == originalHE->faces.size());
    CHECK(resultHE->half_edges.size() == originalHE->half_edges.size());
}
#endif