#include "mesh2halfedge.h"

#include <set>


namespace Library
{
    static std::pair<int64_t, int64_t> makeEdgeKey(int64_t v1, int64_t v2)
    {
        return (v1 < v2) ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
    }

    std::unique_ptr<HalfEdgeMesh> mesh2halfedge::convert(const Triangulation& tri)
    {
        auto mesh = std::make_unique<HalfEdgeMesh>();

        // 1. Initialize Vertices
        for (const auto& pos : tri.vertices)
        {
            mesh->vertices.push_back({ pos, SafeNull });
        }

        // 1. Initialize Edges
        std::map<std::pair<int64_t, int64_t>, int64_t> edgeMap;
        for (size_t i = 0; i < tri.indices.size(); i += 3)
        {
            int64_t v[3] = { static_cast<int64_t>(tri.indices[i]), static_cast<int64_t>(tri.indices[i + 1]), static_cast<int64_t>(tri.indices[i + 2]) };

            if (v[0] == v[1] || v[1] == v[2] || v[0] == v[2])
                continue;

            int64_t face_index = static_cast<int64_t>(mesh->faces.size());
            mesh->faces.push_back(Face());

            auto a             = std::make_pair(v[0], v[1]);
            auto b             = std::make_pair(v[1], v[2]);
            auto c             = std::make_pair(v[2], v[0]);
            auto a_twin        = std::make_pair(v[1], v[0]);
            auto b_twin        = std::make_pair(v[2], v[1]);
            auto c_twin        = std::make_pair(v[0], v[2]);
            auto search_a      = edgeMap.find(a);
            auto search_b      = edgeMap.find(b);
            auto search_c      = edgeMap.find(c);
            auto search_a_twin = edgeMap.find(a_twin);
            auto search_b_twin = edgeMap.find(b_twin);
            auto search_c_twin = edgeMap.find(c_twin);

            if (search_a == edgeMap.end())
            {
                edgeMap[a] = mesh->half_edges.size();
                mesh->half_edges.push_back(HalfEdge());
            }
            if (search_b == edgeMap.end())
            {
                edgeMap[b] = mesh->half_edges.size();
                mesh->half_edges.push_back(HalfEdge());
            }
            if (search_c == edgeMap.end())
            {
                edgeMap[c] = mesh->half_edges.size();
                mesh->half_edges.push_back(HalfEdge());
            }
            if (search_a_twin == edgeMap.end())
            {
                edgeMap[a_twin] = mesh->half_edges.size();
                mesh->half_edges.push_back(HalfEdge());
            }
            if (search_b_twin == edgeMap.end())
            {
                edgeMap[b_twin] = mesh->half_edges.size();
                mesh->half_edges.push_back(HalfEdge());
            }
            if (search_c_twin == edgeMap.end())
            {
                edgeMap[c_twin] = mesh->half_edges.size();
                mesh->half_edges.push_back(HalfEdge());
            }

            int64_t a_index      = edgeMap[a];
            int64_t b_index      = edgeMap[b];
            int64_t c_index      = edgeMap[c];
            int64_t a_twin_index = edgeMap[a_twin];
            int64_t b_twin_index = edgeMap[b_twin];
            int64_t c_twin_index = edgeMap[c_twin];

            auto& a_edge      = mesh->half_edges[a_index];
            auto& b_edge      = mesh->half_edges[b_index];
            auto& c_edge      = mesh->half_edges[c_index];
            auto& a_twin_edge = mesh->half_edges[a_twin_index];
            auto& b_twin_edge = mesh->half_edges[b_twin_index];
            auto& c_twin_edge = mesh->half_edges[c_twin_index];

            a_edge.face = face_index;
            b_edge.face = face_index;
            c_edge.face = face_index;
            a_edge.next = b_index;
            b_edge.next = c_index;
            c_edge.next = a_index;

            a_edge.target_vertex      = v[1];
            b_edge.target_vertex      = v[2];
            c_edge.target_vertex      = v[0];
            a_edge.twin               = a_twin_index;
            b_edge.twin               = b_twin_index;
            c_edge.twin               = c_twin_index;
            a_twin_edge.target_vertex = v[0];
            b_twin_edge.target_vertex = v[1];
            c_twin_edge.target_vertex = v[2];
            a_twin_edge.twin          = a_index;
            b_twin_edge.twin          = b_index;
            c_twin_edge.twin          = c_index;

            mesh->faces.back().half_edge   = a_index;
            mesh->vertices[v[0]].half_edge = a_index;
            mesh->vertices[v[1]].half_edge = b_index;
            mesh->vertices[v[2]].half_edge = c_index;
        }

        std::map<int64_t, std::vector<int64_t>> vertex2edges;

        std::set<int64_t> someonePointsAt;

        for (size_t i = 0; i < mesh->half_edges.size(); i++)
        {
            auto& edge = mesh->half_edges[i];
            auto& twin = mesh->half_edges[edge.twin];
            vertex2edges[twin.target_vertex].push_back(i);
            someonePointsAt.insert(edge.next);
        }

        for (size_t i = 0; i < mesh->half_edges.size(); i++)
        {
            auto& edge = mesh->half_edges[i];
            if (edge.next == SafeNull)
            {
                const auto& list = vertex2edges[edge.target_vertex];
                for (const auto& x : list)
                {
                    if (!someonePointsAt.contains(x))
                        edge.next = x;
                }
            }
        }

        return mesh;
    }

    // Round Trip: HalfEdgeMesh -> Triangulation
    std::unique_ptr<Triangulation> mesh2halfedge::convert(const HalfEdgeMesh& mesh)
    {
        auto tri = std::make_unique<Triangulation>();

        for (const auto& v : mesh.vertices)
        {
            tri->vertices.push_back(v.position);
        }

        for (const auto& face : mesh.faces)
        {
            if (face.half_edge == SafeNull)
                continue;

            int64_t e0 = face.half_edge;
            int64_t e1 = mesh.half_edges[e0].next;
            int64_t e2 = mesh.half_edges[e1].next;

            tri->indices.push_back(static_cast<uint32_t>(mesh.half_edges[e0].target_vertex));
            tri->indices.push_back(static_cast<uint32_t>(mesh.half_edges[e1].target_vertex));
            tri->indices.push_back(static_cast<uint32_t>(mesh.half_edges[e2].target_vertex));
        }

        return tri;
    }
}

#ifdef ISTESTPROJECT
#include "HalfEdgeHealth.h"
#include "Library/catch.hpp"
using namespace Library;

TEST_CASE("mesh2halfedge: Empty Mesh Health")
{
    Triangulation emptyTri;
    auto          heMesh = mesh2halfedge::convert(emptyTri);

    REQUIRE(heMesh != nullptr);
    // An empty mesh is technically a healthy mesh
    bool health = HalfEdgeHealth::isHealthy(*heMesh);
    REQUIRE(health);
}

TEST_CASE("mesh2halfedge: Single Triangle Health")
{
    Triangulation tri;
    tri.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 }
    };
    tri.indices = { 0, 1, 2 };

    auto heMesh = mesh2halfedge::convert(tri);

    REQUIRE(heMesh != nullptr);
    // Validates:
    // 1. next->next->next brings us back to start
    // 2. target_vertex indices are within bounds
    // 3. twin->twin points back to self
    REQUIRE(HalfEdgeHealth::isHealthy(*heMesh));
}

TEST_CASE("mesh2halfedge: Multi-Face Connectivity Health")
{
    Triangulation tri;
    // Two triangles sharing an edge (1,2)
    tri.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 }
    };
    tri.indices = { 0, 1, 2, 1, 3, 2 };

    auto heMesh = mesh2halfedge::convert(tri);

    REQUIRE(heMesh != nullptr);
    CHECK(heMesh->faces.size() == 2);

    // Check that the shared edge is properly stitched
    REQUIRE(HalfEdgeHealth::isHealthy(*heMesh));
}

TEST_CASE("mesh2halfedge: Round Trip Health Validation")
{
    // Build a more complex shape (a simple pyramid/tetrahedron)
    Triangulation tri;
    tri.vertices = {
        {   0,   0, 0 },
        {   1,   0, 0 },
        { 0.5,   1, 0 },
        { 0.5, 0.5, 1 }
    };
    tri.indices = {
        0, 2, 1, // Base
        0, 1, 3, // Side 1
        1, 2, 3, // Side 2
        2, 0, 3  // Side 3
    };

    // Triangulation -> HalfEdge
    auto heMesh = mesh2halfedge::convert(tri);
    REQUIRE(heMesh != nullptr);
    REQUIRE(HalfEdgeHealth::isHealthy(*heMesh));

    // HalfEdge -> Triangulation
    auto triBack = mesh2halfedge::convert(*heMesh);
    REQUIRE(triBack != nullptr);
    REQUIRE(triBack->indices.size() == 12); // 4 faces * 3 indices

    // Final check: Convert back again to ensure the triangulation was valid
    auto heMeshFinal = mesh2halfedge::convert(*triBack);
    REQUIRE(HalfEdgeHealth::isHealthy(*heMeshFinal));
}

TEST_CASE("mesh2halfedge: Non-Manifold or Degenerate Handling")
{
    Triangulation tri;
    tri.vertices = {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 0, 1, 0 }
    };
    // Degenerate triangle (all same indices)
    tri.indices = { 0, 0, 0 };

    auto heMesh = mesh2halfedge::convert(tri);

    // Depending on your implementation, this might return nullptr
    // or a mesh that isHealthy() returns false for.
    if (heMesh != nullptr)
    {
        // If the converter allows degenerate triangles, the health check
        // should ideally catch the logical inconsistency.
        bool health = HalfEdgeHealth::isHealthy(*heMesh);
        // This is a "knowledge-based" check: decide if your converter
        // should produce healthy meshes even from messy input.
        CHECK_FALSE(health);
    }
}
TEST_CASE("mesh2halfedge: Pyramid Health Diagnostic")
{
    Triangulation tri;
    // 4 vertices forming a tetrahedron
    tri.vertices = {
        {   0,   0, 0 },
        {   1,   0, 0 },
        { 0.5,   1, 0 },
        { 0.5, 0.5, 1 }
    };
    tri.indices = {
        0, 2, 1, // Base
        0, 1, 3, // Side 1
        1, 2, 3, // Side 2
        2, 0, 3  // Side 3
    };

    auto heMesh = mesh2halfedge::convert(tri);
    REQUIRE(heMesh != nullptr);

    // If this fails, let's find out exactly which edge is the culprit
    bool overallHealth = HalfEdgeHealth::isHealthy(*heMesh);

    if (!overallHealth)
    {
        for (int64_t i = 0; i < (int64_t)heMesh->half_edges.size(); ++i)
        {
            const auto& he = heMesh->half_edges[i];

            // Rule 1: Twin Symmetry
            if (he.twin == SafeNull || heMesh->half_edges[he.twin].twin != i)
            {
                UNSCOPED_INFO("Edge " << i << " fails Twin Symmetry. Twin is: " << he.twin);
                CHECK(false);
            }

            // Rule 2: Next pointer continuity
            if (he.face != SafeNull)
            {
                int64_t next_e        = he.next;
                int64_t target_v      = he.target_vertex;
                int64_t next_source_v = heMesh->source(next_e);

                if (target_v != next_source_v)
                {
                    UNSCOPED_INFO("Edge " << i << " continuity fail. Target: " << target_v << " Next Source: " << next_source_v);
                    CHECK(false);
                }
            }
        }

        for (int64_t i = 0; i < (int64_t)heMesh->vertices.size(); ++i)
        {
            if (heMesh->vertices[i].half_edge == SafeNull)
            {
                UNSCOPED_INFO("Vertex " << i << " has no outgoing half_edge.");
                CHECK(false);
            }
        }
    }

    REQUIRE(overallHealth);
}
#endif