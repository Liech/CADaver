#include "LoadCADOperation.h"

#include "Library/CAD/CADShape.h"

namespace Library
{
    std::unique_ptr<CADShape> LoadCADOperation::loadFromFile(const std::string& filename)
    {
        return CADShape::load(filename);
    }
}