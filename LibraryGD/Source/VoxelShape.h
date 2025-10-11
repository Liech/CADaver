#pragma once

#include "TriangleShape.h"
#include <glm/glm.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <memory>

namespace Library
{
    class BinaryVolume;
}

namespace godot
{
    class VoxelShape : public RefCounted
    {
        GDCLASS(VoxelShape, RefCounted)

      protected:
        static void _bind_methods();

      public:
        VoxelShape();
        virtual ~VoxelShape();

        godot::String _to_string() const;

        static Ref<VoxelShape> loadVoxFromFile(const godot::String&);
        bool                   saveVoxToFile(const godot::String&);

      private:
        void                   setData(std::shared_ptr<Library::BinaryVolume>);
        Library::BinaryVolume& getData();
        Library::BinaryVolume& getData() const;
        godot::AABB            getAABB() const;
        godot::Vector3i        getResolution() const;
        bool                   get(const godot::Vector3i&) const;
        void                   set(const godot::Vector3i&, bool);
        bool                   isInside(const godot::Vector3i&) const;
        Ref<TriangleShape>     getTriangulationRound() const;
        Ref<TriangleShape>     getTriangulationBlocky() const;

        std::shared_ptr<Library::BinaryVolume> shape;
    };
}