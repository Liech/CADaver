#pragma once

#include <memory>

namespace Library
{
    class Triangulation;
    class CADShape;

    class TriangulateOperation
    {
      public:
        static std::unique_ptr<Triangulation> triangulate(const CADShape&, double precision = 0.1);
    };
}