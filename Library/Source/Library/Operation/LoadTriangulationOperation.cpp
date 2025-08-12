#include "LoadTriangulationOperation.h"

#include "Library/Triangle/STLWriter.h"
#include "Library/Triangle/Triangulation.h"

#include <StlAPI_Reader.hxx>
#include <gp_Pnt.hxx>

namespace Library
{
    std::unique_ptr<Triangulation> LoadTriangulationOperation::loadFromFile(const std::string& filename)
    {
        auto result = std::make_unique<Triangulation>();

        throw std::runtime_error("Not implemented yet!");

        return std::move(result);
    }
}