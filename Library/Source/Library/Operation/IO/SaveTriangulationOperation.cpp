#include "SaveTriangulationOperation.h"

#include "Library/Triangle/STLWriter.h"
#include "Library/Triangle/Triangulation.h"

namespace Library
{
    void SaveTriangulationOperation::saveToFile(const Triangulation& volume, const std::string& filename)
    {
        volume.saveAsSTL(filename);
    }
}