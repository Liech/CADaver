#include "LoadCADOperation.h"

#include "Library/CAD/CADShape.h"
#include "Triangle/mesh2cad_dumb.h"

namespace Library
{
    LoadCADOperation::LoadCADOperation() {}

    LoadCADOperation::~LoadCADOperation() {}

    std::unique_ptr<CADShape> LoadCADOperation::loadFromFile(const std::string& filename)
    {
        return CADShape::load(filename);
    }

    std::unique_ptr<CADShape> LoadCADOperation::cadify_dumb(const CADacomb::Triangulation& mesh)
    {
        return mesh2cad_dumb::cadify_dumb(mesh);
    }
}