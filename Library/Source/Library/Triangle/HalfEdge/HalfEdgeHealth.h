#pragma once

#include "HalfEdge.h"
#include <string>

namespace Library
{
    struct HalfEdgeMesh;
    class Triangulation;

    class HalfEdgeHealth
    {
      public:
        static std::string toString(const HalfEdgeMesh&);
        static std::string toString(const Triangulation&);
        static std::string createReport(const HalfEdgeMesh& mesh);
        static std::string createReport(const Triangulation& mesh);

        static bool isHealthy(const HalfEdgeMesh& mesh);
        static bool isHealthy(const Triangulation& mesh);
    };
}