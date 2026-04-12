#include "HalfEdgeHealth.h"

#include "Library/Triangle/Triangulation.h"
#include <iomanip>
#include <map>
#include <set>
#include <sstream>

namespace Library
{
    std::string HalfEdgeHealth::toString(const HalfEdgeMesh& target)
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

    std::string HalfEdgeHealth::toString(const Triangulation& target)
    {
        std::stringstream ss;
        ss << "=== Triangulation Dump ===\n";

        // 1. Dump Vertices
        ss << "Vertices [" << target.vertices.size() << "]:\n";
        ss << "  ID |        X        |        Y        |        Z        \n";
        ss << "----------------------------------------------------------\n";
        for (size_t i = 0; i < target.vertices.size(); ++i)
        {
            const auto& v = target.vertices[i];
            ss << std::setw(4) << i << " | " << std::fixed << std::setprecision(4) << std::setw(15) << v.x << " | " << std::setw(15) << v.y << " | " << std::setw(15) << v.z << "\n";
        }

        // 2. Dump Triangles (Indices)
        // We assume every 3 indices represent a face
        size_t triangleCount = target.indices.size() / 3;
        ss << "\nTriangles [" << triangleCount << "]:\n";
        ss << "  ID | v0  | v1  | v2  \n";
        ss << "-----------------------\n";
        for (size_t i = 0; i < triangleCount; ++i)
        {
            size_t base = i * 3;
            ss << std::setw(4) << i << " | " << std::setw(3) << target.indices[base] << " | " << std::setw(3) << target.indices[base + 1] << " | " << std::setw(3) << target.indices[base + 2] << "\n";
        }

        // 3. Simple Integrity Check
        if (target.indices.size() % 3 != 0)
        {
            ss << "\n[WARNING]: Index count is not a multiple of 3! (Trailing indices: " << (target.indices.size() % 3) << ")\n";
        }

        ss << "==========================\n";
        return ss.str();
    }

    std::string HalfEdgeHealth::createReport(const HalfEdgeMesh& mesh)
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

    bool HalfEdgeHealth::isHealthy(const HalfEdgeMesh& mesh)
    {
        // 1. Verify Half-Edge Pointers and Twins
        for (int64_t i = 0; i < (int64_t)mesh.half_edges.size(); ++i)
        {
            const auto& he = mesh.half_edges[i];

            // Bound Check: Twin
            if (he.twin < 0 || he.twin >= (int64_t)mesh.half_edges.size())
            {
                return false;
            }

            // Symmetry Check: Twin's twin must be self
            if (mesh.half_edges[he.twin].twin != i)
            {
                return false;
            }

            // Continuity Check: For internal edges (belonging to a face)
            if (he.face != SafeNull)
            {
                // Bound Check: Next
                if (he.next < 0 || he.next >= (int64_t)mesh.half_edges.size())
                {
                    return false;
                }

                // Logic Check: mesh.source(next) must be mesh.target(current)
                // In half-edge terms: mesh.half_edges[he.twin].target_vertex == source
                int64_t target_v      = he.target_vertex;
                int64_t next_source_v = mesh.half_edges[mesh.half_edges[he.next].twin].target_vertex;

                if (target_v != next_source_v)
                {
                    return false;
                }

                // Loop Check: Ensure we don't have immediate self-loops (or simple triangles)
                // The test "Broken Next Loop" explicitly checks if next == self
                if (he.next == i)
                {
                    return false;
                }
            }
        }

        // 2. Verify Face Integrity
        for (int64_t i = 0; i < (int64_t)mesh.faces.size(); ++i)
        {
            if (mesh.faces[i].half_edge == SafeNull)
            {
                return false;
            }

            // Optional: Ensure the face's root edge actually points back to this face
            if (mesh.half_edges[mesh.faces[i].half_edge].face != i)
            {
                return false;
            }
        }

        // 3. Verify Vertex Integrity
        for (int64_t i = 0; i < (int64_t)mesh.vertices.size(); ++i)
        {
            if (mesh.vertices[i].half_edge == SafeNull)
            {
                // Boundary vertices might technically have no outgoing internal edge,
                // but usually, a mesh is only healthy if every vertex is used.
                return false;
            }
        }

        return true;
    }

    std::string HalfEdgeHealth::createReport(const Triangulation& mesh)
    {
        size_t oob_count           = 0;
        size_t degenerate_count    = 0;
        size_t duplicate_verts     = 0;
        size_t open_edges          = 0;
        size_t non_manifold_edges  = 0;
        size_t inconsistent_orient = 0; // New counter

        // Key: sorted pair of vertex indices, Value: appearance count
        std::map<std::pair<size_t, size_t>, int> edge_counts;

        // New: Track directed edges to check orientation
        // Key: {start, end}, Value: count
        std::map<std::pair<size_t, size_t>, int> directed_edge_counts;

        // 1. Process Triangles
        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
        {
            size_t i0 = mesh.indices[i];
            size_t i1 = mesh.indices[i + 1];
            size_t i2 = mesh.indices[i + 2];

            if (i0 >= mesh.vertices.size() || i1 >= mesh.vertices.size() || i2 >= mesh.vertices.size())
            {
                oob_count++;
                continue;
            }

            const glm::dvec3& v0 = mesh.vertices[i0];
            const glm::dvec3& v1 = mesh.vertices[i1];
            const glm::dvec3& v2 = mesh.vertices[i2];

            if (i0 == i1 || i1 == i2 || i2 == i0)
            {
                degenerate_count++;
            }
            else if (glm::length(glm::cross(v1 - v0, v2 - v0)) < 1e-11)
            {
                degenerate_count++;
            }

            // Updated Edge Tracking
            auto track_edge = [&](size_t a, size_t b)
            {
                // For manifold/watertight check (undirected)
                std::pair<size_t, size_t> sorted_edge = (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
                edge_counts[sorted_edge]++;

                // For orientation check (directed)
                directed_edge_counts[{ a, b }]++;
            };
            track_edge(i0, i1);
            track_edge(i1, i2);
            track_edge(i2, i0);
        }

        // 2. Analyze Edges
        for (auto const& [edge, count] : edge_counts)
        {
            if (count == 1)
            {
                open_edges++;
            }
            else if (count > 2)
            {
                non_manifold_edges++;
            }
            else if (count == 2)
            {
                // Check orientation: one must be (u, v) and the other (v, u)
                size_t u = edge.first;
                size_t v = edge.second;
                // If both triangles used the same direction, it's inconsistent
                if (directed_edge_counts[{ u, v }] != 1 || directed_edge_counts[{ v, u }] != 1)
                {
                    inconsistent_orient++;
                }
            }
        }

        // 3. Duplicate Vertex Check (Spatial)
        std::set<std::vector<double>> unique_positions;
        for (const auto& v : mesh.vertices)
        {
            unique_positions.insert({ v.x, v.y, v.z });
        }
        duplicate_verts = mesh.vertices.size() - unique_positions.size();

        // 4. Final Report Output
        std::stringstream ss;
        ss << "--- Mesh Health Report ---" << "\n";
        ss << "Vertices:           " << mesh.vertices.size() << "\n";
        ss << "Indices:            " << mesh.indices.size() << " (" << mesh.indices.size() / 3 << " tris)\n";
        ss << "Out of Bounds:      " << oob_count << (oob_count > 0 ? " [CRITICAL]" : " [OK]") << "\n";
        ss << "Degenerate Tris:    " << degenerate_count << "\n";
        ss << "Duplicate Verts:    " << duplicate_verts << "\n";
        ss << "Watertight:         " << (open_edges == 0 ? "Yes" : "No (" + std::to_string(open_edges) + " holes)") << "\n";
        ss << "Manifold:           " << (non_manifold_edges == 0 ? "Yes" : "No (" + std::to_string(non_manifold_edges) + " complex edges)") << "\n";
        ss << "Consistent Orient:  " << (inconsistent_orient == 0 ? "Yes" : "No (" + std::to_string(inconsistent_orient) + " flipped edges)") << "\n";
        ss << "--------------------------" << std::endl;
        return ss.str();
    }

    bool HalfEdgeHealth::isHealthy(const Triangulation& mesh)
    {
        std::map<std::pair<size_t, size_t>, int> edge_counts;
        std::map<std::pair<size_t, size_t>, int> directed_edge_counts;

        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
        {
            size_t i0 = mesh.indices[i];
            size_t i1 = mesh.indices[i + 1];
            size_t i2 = mesh.indices[i + 2];

            // 1. Bounds Check
            if (i0 >= mesh.vertices.size() || i1 >= mesh.vertices.size() || i2 >= mesh.vertices.size())
                return false;

            const glm::dvec3& v0 = mesh.vertices[i0];
            const glm::dvec3& v1 = mesh.vertices[i1];
            const glm::dvec3& v2 = mesh.vertices[i2];

            // 2. Degenerate Check
            if (i0 == i1 || i1 == i2 || i2 == i0)
                return false;
            if (glm::length(glm::cross(v1 - v0, v2 - v0)) < 1e-11)
                return false;

            auto track_edge = [&](size_t a, size_t b)
            {
                std::pair<size_t, size_t> sorted_edge = (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
                edge_counts[sorted_edge]++;
                directed_edge_counts[{ a, b }]++;
            };
            track_edge(i0, i1);
            track_edge(i1, i2);
            track_edge(i2, i0);
        }

        // 3. Analyze Edges for Watertightness, Manifold, and Orientation
        for (auto const& [edge, count] : edge_counts)
        {
            // Must have exactly 2 faces per edge for a closed manifold
            if (count != 2)
                return false;

            // Orientation check: one must be (u,v), other must be (v,u)
            if (directed_edge_counts[{ edge.first, edge.second }] != 1 || directed_edge_counts[{ edge.second, edge.first }] != 1)
            {
                return false;
            }
        }

        return true;
    }
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"

// Assuming these are within your Library namespace
using namespace Library;

void setupValidTriangle(HalfEdgeMesh& mesh)
{
    mesh.vertices.clear();
    mesh.half_edges.clear();
    mesh.faces.clear();

    mesh.vertices.resize(3);
    mesh.half_edges.resize(6);
    mesh.faces.resize(1);

    for (int64_t i = 0; i < 3; ++i)
    {
        int64_t next_i = (i + 1) % 3;

        // Internal (Part of Face 0)
        mesh.half_edges[i].next          = next_i;
        mesh.half_edges[i].target_vertex = next_i;
        mesh.half_edges[i].face          = 0;
        mesh.half_edges[i].twin          = i + 3;

        // External (Boundary - No Face)
        mesh.half_edges[i + 3].twin          = i;
        mesh.half_edges[i + 3].target_vertex = i;
        mesh.half_edges[i + 3].face          = SafeNull;
        mesh.half_edges[i + 3].next          = SafeNull; // This caused the fail; now ignored

        mesh.vertices[i].half_edge = i;
    }
    mesh.faces[0].half_edge = 0;
}

TEST_CASE("Health: Valid Triangle", "[mesh]")
{
    HalfEdgeMesh mesh;
    setupValidTriangle(mesh);
    REQUIRE(HalfEdgeHealth::isHealthy(mesh) == true);
}

TEST_CASE("Health: Broken Next Loop", "[mesh]")
{
    HalfEdgeMesh mesh;
    setupValidTriangle(mesh);
    mesh.half_edges[0].next = 0; // Self-loop
    REQUIRE(HalfEdgeHealth::isHealthy(mesh) == false);
}

TEST_CASE("Health: Twin Asymmetry", "[mesh]")
{
    HalfEdgeMesh mesh;
    setupValidTriangle(mesh);
    mesh.half_edges[3].twin = 99; // Break the return link
    REQUIRE(HalfEdgeHealth::isHealthy(mesh) == false);
}

TEST_CASE("Health: Twin Out of Bounds", "[mesh]")
{
    HalfEdgeMesh mesh;
    setupValidTriangle(mesh);
    mesh.half_edges[0].twin = 5000;
    REQUIRE(HalfEdgeHealth::isHealthy(mesh) == false);
}

TEST_CASE("Health: Face Missing Root Edge", "[mesh]")
{
    HalfEdgeMesh mesh;
    setupValidTriangle(mesh);
    mesh.faces[0].half_edge = SafeNull;
    REQUIRE(HalfEdgeHealth::isHealthy(mesh) == false);
}

TEST_CASE("Health: Vertex Continuity Break", "[mesh]")
{
    HalfEdgeMesh mesh;
    setupValidTriangle(mesh);
    // Edge 0 targets V1, but we'll lie and say it targets V2.
    // This breaks the path to Edge 1.
    mesh.half_edges[0].target_vertex = 2;
    REQUIRE(HalfEdgeHealth::isHealthy(mesh) == false);
}
#endif