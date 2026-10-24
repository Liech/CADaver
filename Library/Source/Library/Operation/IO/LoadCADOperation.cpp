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

#ifdef ISTESTPROJECT
#include "CADacomb/Triangle/Triangulation.h"
#include "Library/Operation/TriangulateOperation.h"
#include "Library/catch.hpp"
#include <filesystem>

TEST_CASE("LoadCADOperation/LoadAndTriangulateStep")
{
    std::filesystem::path stepPath = "Bo.step";
    for (int i = 0; i < 6; i++)
    {
        if (std::filesystem::exists(stepPath))
        {
            break;
        }
        stepPath = ".." / stepPath;
    }
    REQUIRE(std::filesystem::exists(stepPath));

    auto shape = Library::LoadCADOperation::loadFromFile(stepPath.string());
    REQUIRE(shape != nullptr);

    auto tri = Library::TriangulateOperation::triangulate(*shape, 0.1);
    REQUIRE(tri != nullptr);
    REQUIRE(!tri->vertices.empty());
    REQUIRE(!tri->indices.empty());
}
#endif