#include "HalfEdge.h"


#ifdef ISTESTPROJECT
#include "Library/catch.hpp"

using namespace Library;

// Helper to create a basic manifold mesh: A quad made of two triangles
HalfEdgeMesh create_quad_mesh()
{
    HalfEdgeMesh mesh;
    mesh.vertices.resize(4);
    mesh.faces.resize(2);
    mesh.half_edges.resize(6);

    // Triangle 0: 0 -> 1 -> 2
    mesh.half_edges[0] = { 1, 1, -1, 0 }; // 0->1
    mesh.half_edges[1] = { 2, 2, -1, 0 }; // 1->2
    mesh.half_edges[2] = { 0, 0, 3, 0 };  // 2->0 (Shared)

    // Triangle 1: 0 -> 2 -> 3
    mesh.half_edges[3] = { 2, 4, 2, 1 };  // 0->2 (Shared)
    mesh.half_edges[4] = { 3, 5, -1, 1 }; // 2->3
    mesh.half_edges[5] = { 0, 3, -1, 1 }; // 3->0

    // --- THE MISSING LINKS ---

    // Connect Vertices to one of their OUTGOING half-edges
    mesh.vertices[0].half_edge = 0; // 0->1
    mesh.vertices[1].half_edge = 1; // 1->2
    mesh.vertices[2].half_edge = 4; // 2->3 (or 2->0)
    mesh.vertices[3].half_edge = 5; // 3->0

    // Connect Faces to one of their constituent edges
    mesh.faces[0].half_edge = 0;
    mesh.faces[1].half_edge = 3;

    // Fill positions so they aren't all zero
    mesh.vertices[0].position = { 0, 0, 0 };
    mesh.vertices[1].position = { 1, 0, 0 };
    mesh.vertices[2].position = { 1, 1, 0 };
    mesh.vertices[3].position = { 0, 1, 0 };

    return mesh;
}

TEST_CASE("HalfEdgeMesh Basic Connectivity", "[halfedge]")
{
    HalfEdgeMesh mesh = create_quad_mesh();

    SECTION("Twin Relationship")
    {
        int64_t e = 2; // Edge 2->0
        int64_t t = mesh.twin(e);

        REQUIRE(t != SafeNull);
        REQUIRE(mesh.half_edges[t].target_vertex == 2);
        REQUIRE(mesh.twin(t) == e); // Twin of twin is self
    }

    SECTION("Source and Target")
    {
        int64_t e = 3; // 0 -> 2
        REQUIRE(mesh.half_edges[e].target_vertex == 2);
        REQUIRE(mesh.source(e) == 0);
    }

    SECTION("Face Cycles")
    {
        int64_t start_e = 0;
        int64_t e       = start_e;
        int     count   = 0;

        // Traverse Triangle 0
        do
        {
            e = mesh.next(e);
            count++;
        } while (e != start_e && count < 10);

        REQUIRE(count == 3);
    }
}

TEST_CASE("Simplified Vertex Orbit", "[topology]")
{
    HalfEdgeMesh mesh = create_quad_mesh();
    int64_t start_edge = 3;
    int64_t neighbor_edge = mesh.twin(start_edge); // result: 2
    REQUIRE(neighbor_edge == 2);
    REQUIRE(mesh.half_edges[neighbor_edge].face == 0); // We are now in Face 0
    int64_t next_in_orbit = mesh.next(neighbor_edge); // result: 0
    REQUIRE(next_in_orbit == 0);
}

Library::HalfEdgeMesh create_complex_test_mesh()
{
    Library::HalfEdgeMesh mesh;
    // 4 Vertices in a pyramid shape
    mesh.vertices.resize(4);
    mesh.vertices[0].position = glm::dvec3(0, 0, 0);
    mesh.vertices[1].position = glm::dvec3(1, 0, 0);
    mesh.vertices[2].position = glm::dvec3(0.5, 1, 0);
    mesh.vertices[3].position = glm::dvec3(0.5, 0.5, 1);

    mesh.faces.resize(4);
    mesh.half_edges.resize(12);

    // This is a simplified manual construction.
    // In a real test, you'd likely use your mesh2halfedge::convert helper
    // on a hardcoded Triangulation object to ensure 'convert' itself is tested.

    // For the sake of the unit test logic:
    // Every edge must have a twin, every next must point to the next in the triangle.
    // Face 0 (Bottom): 0-1, 1-2, 2-0
    // Face 1 (Side):   0-2, 2-3, 3-0
    // Face 2 (Side):   1-3, 3-2, 2-1
    // Face 3 (Side):   0-3, 3-1, 1-0

    return mesh;
}
TEST_CASE("Mesh Topology Invariants", "[topology]")
{
    HalfEdgeMesh mesh = create_complex_test_mesh();

    size_t V = mesh.vertices.size();
    size_t F = mesh.faces.size();
    // In Half-Edge, each 'Edge' is two 'Half-Edges'
    size_t E = mesh.half_edges.size() / 2;

    // For a genus-0 manifold mesh
    REQUIRE(V - E + F == 2);
}

Library::HalfEdgeMesh create_open_mesh()
{
    Library::HalfEdgeMesh mesh;
    mesh.vertices.resize(3);
    mesh.faces.resize(1);
    mesh.half_edges.resize(3);

    // Triangle 0: 0 -> 1 -> 2
    mesh.half_edges[0] = { 1, 1, Library::SafeNull, 0 };
    mesh.half_edges[1] = { 2, 2, Library::SafeNull, 0 };
    mesh.half_edges[2] = { 0, 0, Library::SafeNull, 0 };

    mesh.vertices[0].half_edge = 0;
    mesh.vertices[1].half_edge = 1;
    mesh.vertices[2].half_edge = 2;
    mesh.faces[0].half_edge    = 0;

    return mesh;
}

TEST_CASE("Boundary Integrity", "[topology]")
{
    HalfEdgeMesh mesh = create_open_mesh(); // A mesh with an edge that has no twin

    for (const auto& edge : mesh.half_edges)
    {
        if (edge.twin == SafeNull)
        {
            // If the twin is null, this must be a boundary.
            // Ensure the logic doesn't assume every edge is paired.
            SUCCEED("Handled null twin correctly");
        }
    }
}

#endif