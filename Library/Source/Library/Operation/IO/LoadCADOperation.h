#pragma once

#include <memory>
#include <string>

namespace Library
{
    class CADShape;

    class LoadCADOperation
    {
      public:
        static std::unique_ptr<CADShape> loadFromFile(const std::string& filename);
    };
}