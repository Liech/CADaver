#pragma once

#include "CADShape.h"
#include <glm/glm.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <memory>
#include <string>

namespace Library
{
    class CADVertex;
    class CADEdge;
}

namespace godot
{
    class CADVertex;

    class CADEdge : public godot::CADShape
    {
        GDCLASS(CADEdge, godot::CADShape)

      protected:
        static void _bind_methods();

      public:
        CADEdge();
        virtual ~CADEdge();

      private:
        const Library::CADEdge& get() const;
        Library::CADEdge&       get();

        Ref<godot::CADVertex> getCADStart() const;
        Ref<godot::CADVertex> getCADEnd() const;
        godot::String         getCADOrientation() const;
    };
}