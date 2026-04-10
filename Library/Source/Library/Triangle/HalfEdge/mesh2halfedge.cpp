#include "mesh2halfedge.h"

#include <sstream>
#include <iomanip>

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
               << ") | out_edge: " << target.vertices[i].half_edge
               << "\n";
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
}

#ifdef ISTESTPROJECT
#include "Library/catch.hpp"

TEST_CASE("mesh2halfedge/Base")
{
    REQUIRE(true);
}

#endif