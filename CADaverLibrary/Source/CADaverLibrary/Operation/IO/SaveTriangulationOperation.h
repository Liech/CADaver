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
    class SaveTriangulationOperation
    {
      public:
        SaveTriangulationOperation();
        virtual ~SaveTriangulationOperation();

        static void saveToFile(const CADacomb::Triangulation& triangulation, const std::string& filename);
        static void saveToStream(const CADacomb::Triangulation& triangulation, std::ostream& stream);
    };
}
