#pragma once

#include <memory>
#include <string>

namespace CADacomb
{
    class Triangulation;
}

namespace Library
{
    class SaveTriangulationOperation
    {
      public:
        static void saveToFile(const CADacomb::Triangulation&, const std::string& filename);
    };
}
