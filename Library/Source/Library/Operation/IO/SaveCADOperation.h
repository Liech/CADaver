#pragma once

#include <memory>
#include <string>

namespace Library
{
    class CADShape;

    class SaveCADOperation 
    {
      public:
        static void saveToFile(const CADShape&, const std::string& filename);
    };
}