#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Library
{
    class CADShape;

    class ExtrudeOperation
    {
      public:
        static std::unique_ptr<CADShape> extrude(const CADShape&, const glm::dvec3& direction);

      private:
    };
}