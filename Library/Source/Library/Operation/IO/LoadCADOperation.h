#pragma once

#include <memory>
#include <string>

namespace CADacomb
{
    class Triangulation;
}

namespace Library
{
    class CADShape;

    class LoadCADOperation
    {
      public:
        LoadCADOperation();
        virtual ~LoadCADOperation();

        static std::unique_ptr<CADShape> loadFromFile(const std::string& filename);
        static std::unique_ptr<CADShape> cadify_dumb(const CADacomb::Triangulation& mesh);
    };
}