#pragma once

#include <memory>
#include <string>

namespace CADacomb
{
    class Triangulation;
}

namespace Library
{
    class LoadTriangulationOperation
    {
      public:
        static std::unique_ptr<CADacomb::Triangulation> loadFromFile(const std::string& filename);
    };
}
