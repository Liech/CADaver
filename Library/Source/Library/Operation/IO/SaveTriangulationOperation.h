#pragma once

#include <memory>
#include <string>

namespace Library
{
    class Triangulation;

    class SaveTriangulationOperation
    {
      public:
        static void saveToFile(const Triangulation&, const std::string& filename);
    };
}