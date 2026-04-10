#pragma once

#include <memory>

namespace Library
{
    class Triangulation;
    struct HalfEdgeMesh;

    class mesh2halfedge
    {
      public:
        static std::unique_ptr<HalfEdgeMesh>  convert(const Triangulation&);
        static std::unique_ptr<Triangulation> convert(const HalfEdgeMesh&);
    };
}