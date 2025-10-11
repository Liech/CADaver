#include "LoadTriangulationOperation.h"

#include "Library/Triangle/STLWriter.h"
#include "Library/Triangle/Triangulation.h"

#include <StlAPI_Reader.hxx>
#include <gp_Pnt.hxx>

namespace Library
{
    std::unique_ptr<Triangulation> LoadTriangulationOperation::loadFromFile(const std::string& filename)
    {
        return Triangulation::fromSTLFile(filename);
    }
}