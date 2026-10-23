#include "SaveTriangulationOperation.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Triangle/Util/STLWriter.h"

namespace Library
{
    void SaveTriangulationOperation::saveToFile(const CADacomb::Triangulation& volume, const std::string& filename)
    {
        volume.saveAsSTL(filename);
    }
}
