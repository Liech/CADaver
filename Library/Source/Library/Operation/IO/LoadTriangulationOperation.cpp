#include "LoadTriangulationOperation.h"

#include "CADacomb/Triangle/Triangulation.h"

namespace Library
{
    LoadTriangulationOperation::LoadTriangulationOperation() {}

    LoadTriangulationOperation::~LoadTriangulationOperation() {}

    std::unique_ptr<CADacomb::Triangulation> LoadTriangulationOperation::loadFromFile(const std::string& filename)
    {
        return CADacomb::Triangulation::fromSTLFile(filename);
    }

    std::unique_ptr<CADacomb::Triangulation> LoadTriangulationOperation::loadFromStream(std::istream& stream)
    {
        return CADacomb::Triangulation::fromSTLStream(stream);
    }
}
