#include "SaveCADOperation.h"

#include "Library/CAD/CADShape.h"

namespace Library
{
    void SaveCADOperation::saveToFile(const CADShape& shape, const std::string& filename)
    {
        shape.save(filename);
    }
}