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
    class VoxelShape;
    class CADShape;

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
        godot::Array normal_cluster(godot::Callable grow_func, bool holes);
        godot::Array cluster_border(godot::Array);
        Vector3      getVertex(uint64_t);

        godot::String toHalfEdgeString() const;
        godot::String getHalfEdgeReport() const;
        godot::String getMeshReport() const;

        Ref<VoxelShape> toVoxel(const Vector3i& resolution) const;
        Ref<CADShape>   toCad_dumb() const;
        Ref<ArrayMesh>  getMesh() const;
        godot::AABB     getAABB() const;

        std::shared_ptr<Library::Triangulation> shape;
    };
}