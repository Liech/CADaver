#include "LoadCADOperation.h"

#include "Library/CAD/CADShape.h"
#include "Triangle/mesh2cad_dumb.h"

namespace Library
{
    std::unique_ptr<CADShape> LoadCADOperation::loadFromFile(const std::string& filename)
    {
        return CADShape::load(filename);
    }

    std::unique_ptr<CADShape> LoadCADOperation::cadify_dumb(const Triangulation& mesh)
    {
        return mesh2cad_dumb::cadify_dumb(mesh);
    }
}