#include "SaveCADOperation.h"

#include "CADaverLibrary/CAD/CADShape.h"

namespace Library
{
    void SaveCADOperation::saveToFile(const CADShape& shape, const std::string& filename)
    {
        shape.save(filename);
    }
}