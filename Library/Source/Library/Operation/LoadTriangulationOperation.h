#pragma once

#include <memory>
#include <string>

namespace Library
{
    class Triangulation;

    class LoadTriangulationOperation
    {
      public:
        static std::unique_ptr<Triangulation> loadFromFile(const std::string& filename);
    };
}