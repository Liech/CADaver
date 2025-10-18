#include "TriangleShape.h"

#include "Library/Triangle/Triangulation.h"
#include <godot_cpp/classes/surface_tool.hpp>

namespace godot
{
    void TriangleShape::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("save_tri_to_file", "filename"), &TriangleShape::saveTriToFile);
        ClassDB::bind_static_method("TriangleShape", D_METHOD("load_tri_from_file", "filename"), &TriangleShape::loadTriFromFile);
        ClassDB::bind_method(D_METHOD("get_array_mesh"), &TriangleShape::getMesh);
        ClassDB::bind_method(D_METHOD("get_tri_aabb"), &TriangleShape::getAABB);
    }

    TriangleShape::TriangleShape()
    {
        shape = std::make_unique<Library::Triangulation>();
    }

    TriangleShape::~TriangleShape() {}

    godot::String TriangleShape::_to_string() const
    {
        std::string result = "{ TriangleShape: " + std::to_string(shape->vertices.size()) + "vtex - " + std::to_string(shape->indices.size()) + "idex " + "}";
        return result.c_str();
    }

    Ref<TriangleShape> TriangleShape::loadTriFromFile(const godot::String& str)
    {
        std::string                             filename = std::string(str.utf8());
        std::shared_ptr<Library::Triangulation> result   = Library::Triangulation::fromSTLFile(filename);
        if (result)
        {
            auto s = godot::Ref<godot::TriangleShape>();
            s.instantiate();
            s->setData(result);
            return s;
        }
        return Ref<TriangleShape>();
    }

    bool TriangleShape::saveTriToFile(const godot::String& str)
    {
        std::string filename = std::string(str.utf8());
        shape->saveAsSTL(filename);
        return true;
    }

    void TriangleShape::setData(std::shared_ptr<Library::Triangulation> vol)
    {
        shape = vol;
    }

    Library::Triangulation& TriangleShape::getData()
    {
        return *shape;
    }

    Library::Triangulation& TriangleShape::getData() const
    {
        return *shape;
    }

    Ref<ArrayMesh> TriangleShape::getMesh() const
    {
        if (!shape)
            return nullptr;
        const auto& mesh = *shape;

        Ref<SurfaceTool> st;
        st.instantiate();
        st->begin(Mesh::PRIMITIVE_TRIANGLES);

        for (size_t i = 0; i < mesh.vertices.size(); ++i)
        {
            auto v = Vector3((real_t)mesh.vertices[i].x, (real_t)mesh.vertices[i].y, (real_t)mesh.vertices[i].z);
            st->add_vertex(v);
        }
        PackedInt32Array godot_indices;
        godot_indices.resize(mesh.indices.size());
        for (size_t i = 0; i < mesh.indices.size(); i++)
        {
            st->add_index(mesh.indices[i]);
        }
        st->generate_normals();
        Ref<ArrayMesh> result = st->commit();
        return result;
    }

    godot::AABB TriangleShape::getAABB() const
    {
        auto       inf = std::numeric_limits<double>::infinity();
        glm::dvec3 min = glm::dvec3(inf, inf, inf);
        glm::dvec3 max = glm::dvec3(-inf, -inf, -inf);

        for (const auto& x : shape->vertices)
        {
            min.x = std::min(x.x, min.x);
            min.y = std::min(x.y, min.y);
            min.z = std::min(x.z, min.z);
            max.x = std::max(x.x, max.x);
            max.y = std::max(x.y, max.y);
            max.z = std::max(x.z, max.z);
        }

        Vector3 position = Vector3(min.x, min.y, min.z);
        Vector3 size     = Vector3(max.x - min.x, max.y - min.y, max.z - min.z);
        return AABB(position, size);
    }

}