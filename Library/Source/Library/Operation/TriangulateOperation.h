#pragma once

#include <memory>

namespace Library
{
    class Triangulation;
    class BinaryVolume;
    class CADShape;

    class TriangulateOperation
    {
      public:
        static std::unique_ptr<Triangulation> triangulate(const CADShape&, double precision = 0.1);
        static std::unique_ptr<Triangulation> triangulateRound(const BinaryVolume&);
        static std::unique_ptr<Triangulation> triangulateBlocky(const BinaryVolume&);
    };
}