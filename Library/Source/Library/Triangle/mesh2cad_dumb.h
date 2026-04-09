#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Library
{
    class CADShape;
    class Triangulation;

    class mesh2cad_dumb
    {
      public:
        static std::unique_ptr<CADShape> cadify_dumb(const Triangulation& mesh);
    };
}