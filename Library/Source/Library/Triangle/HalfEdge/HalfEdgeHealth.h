#pragma once

#include <string>
#include "HalfEdge.h"

namespace Library
{
    struct HalfEdgeMesh;
    class Triangulation;

    class HalfEdgeHealth
    {
      public:
        static std::string toString(const HalfEdgeMesh&);
        static std::string createReport(const HalfEdgeMesh& mesh);
        static std::string createReport(const Triangulation& mesh);

        //This function performs a topological integrity check to ensure the mesh's pointers are logically consistent. It verifies that every half-edge has a valid twin pointing back to it, ensures face loops are geometrically continuous without gaps, and confirms that all vertices and faces are properly connected to the edge network without isolated or null references. Essentially, it guarantees the mesh structure is "watertight" and safe for traversal.
        static bool        isHealthy(const HalfEdgeMesh& mesh);
    };
}