#include "SaveTriangulationOperation.h"

#include "CADacomb/Triangle/Triangulation.h"

namespace Library
{
    SaveTriangulationOperation::SaveTriangulationOperation() {}

    SaveTriangulationOperation::~SaveTriangulationOperation() {}

    void SaveTriangulationOperation::saveToFile(const CADacomb::Triangulation& triangulation, const std::string& filename)
    {
        triangulation.saveAsSTL(filename);
    }

    void SaveTriangulationOperation::saveToStream(const CADacomb::Triangulation& triangulation, std::ostream& stream)
    {
        triangulation.saveAsSTL(stream);
    }
}
