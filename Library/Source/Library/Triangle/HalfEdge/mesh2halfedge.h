#pragma once

#include "HalfEdge.h"
#include "Library/Triangle/Triangulation.h"
#include <memory>

namespace Library
{
    class mesh2halfedge
    {
      public:
        static std::unique_ptr<HalfEdgeMesh>  convert(const Triangulation&);
        static std::unique_ptr<Triangulation> convert(const HalfEdgeMesh&);
        static std::string                    toString(const HalfEdgeMesh&);
        static std::string                    createReport(const HalfEdgeMesh& mesh);
    };
}