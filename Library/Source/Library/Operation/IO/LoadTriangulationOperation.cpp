#include "LoadTriangulationOperation.h"

#include "CADacomb/Triangle/Triangulation.h"
#include "CADacomb/Triangle/Util/STLWriter.h"

#include <StlAPI_Reader.hxx>
#include <gp_Pnt.hxx>

namespace Library
{
    std::unique_ptr<CADacomb::Triangulation> LoadTriangulationOperation::loadFromFile(const std::string& filename)
    {
        return CADacomb::Triangulation::fromSTLFile(filename);
    }
}
