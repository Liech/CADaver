#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Library
{
    const int64_t SafeNull = -1;

    struct HalfEdge
    {
        int64_t target_vertex = SafeNull; // Vertex at the end of this half-edge
        int64_t next          = SafeNull; // Next half-edge in CCW order around the face
        int64_t twin          = SafeNull; // Opposite half-edge
        int64_t face          = SafeNull; // Face this half-edge belongs to
    };

    struct Vertex
    {
        glm::dvec3 position;
        int64_t    half_edge = SafeNull; // One of the outgoing half-edges
    };

    struct Face
    {
        int64_t half_edge = SafeNull; // One of the half-edges forming the boundary
    };

    class HalfEdgeMesh
    {
      public:
        std::vector<Vertex>   vertices;
        std::vector<Face>     faces;
        std::vector<HalfEdge> half_edges;

        int64_t source(int64_t e) const
        {
            return half_edges[half_edges[e].twin].target_vertex;
        }

        int64_t next(int64_t e) const
        {
            return half_edges[e].next;
        }
        int64_t twin(int64_t e) const
        {
            return half_edges[e].twin;
        }
    };
}