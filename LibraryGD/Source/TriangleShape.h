#pragma once

#include <glm/glm.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <memory>

namespace Library
{
    class Triangulation;
}

namespace godot
{
    class TriangleShape : public RefCounted
    {
        GDCLASS(TriangleShape, RefCounted)

      protected:
        static void _bind_methods();

      public:
        TriangleShape();
        virtual ~TriangleShape();

        godot::String _to_string() const;

        static Ref<TriangleShape> loadTriFromFile(const godot::String&);
        bool                      saveTriToFile(const godot::String&);

        void                    setData(std::shared_ptr<Library::Triangulation>);
        Library::Triangulation& getData();
        Library::Triangulation& getData() const;

      private:
        Ref<ArrayMesh> getMesh() const;

        std::shared_ptr<Library::Triangulation> shape;
    };
}