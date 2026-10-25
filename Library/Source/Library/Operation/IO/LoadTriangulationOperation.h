#pragma once

#include <iosfwd>
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
        LoadTriangulationOperation();
        virtual ~LoadTriangulationOperation();

        static std::unique_ptr<CADacomb::Triangulation> loadFromFile(const std::string& filename);
        static std::unique_ptr<CADacomb::Triangulation> loadFromStream(std::istream& stream);
    };
}
