#pragma once

#include "Triangulation.h"

namespace Library
{
    class BaseShapeGenerator
    {
      public:
        static Triangulation cube();
        static Triangulation pyramid();
        static Triangulation cylinder(size_t segments, float height, float radius);
        static Triangulation cubeWithHole(size_t segments, float radius = 0.3f);
        static Triangulation cone(size_t segments, float radius, float height);

    };
}
