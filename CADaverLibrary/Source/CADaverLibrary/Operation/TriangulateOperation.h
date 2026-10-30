#pragma once

#include <memory>

namespace CADacomb
{
    class Triangulation;
    class BinaryVolume;
}

namespace Library
{
    class CADShape;

    class TriangulateOperation
    {
      public:
        static std::unique_ptr<CADacomb::Triangulation> triangulate(const CADShape&, double precision = 0.1);
        static std::unique_ptr<CADacomb::Triangulation> triangulateRound(const CADacomb::BinaryVolume&);
        static std::unique_ptr<CADacomb::Triangulation> triangulateBlocky(const CADacomb::BinaryVolume&);
    };
}