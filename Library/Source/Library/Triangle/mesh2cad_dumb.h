#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace CADacomb
{
    class Triangulation;
}

namespace Library
{
    class CADShape;

    class mesh2cad_dumb
    {
      public:
        static std::unique_ptr<CADShape> cadify_dumb(const CADacomb::Triangulation& mesh);
    };
}