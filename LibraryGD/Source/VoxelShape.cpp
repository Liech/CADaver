#include "VoxelShape.h"

#include "Library/Operation/LoadVoxelOperation.h"
#include "Library/Operation/SaveVoxelOperation.h"
#include "Library/Operation/TriangulateOperation.h"
#include "Library/Triangle/Triangulation.h"
#include "Library/Voxel/BinaryVolume.h"
#include <godot_cpp/classes/surface_tool.hpp>

namespace godot
{
    void VoxelShape::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("save_vox_to_file", "filename"), &VoxelShape::saveVoxToFile);
        ClassDB::bind_static_method("VoxelShape", D_METHOD("load_vox_from_file", "filename"), &VoxelShape::loadVoxFromFile);
        ClassDB::bind_method(D_METHOD("get_vox_aabb"), &VoxelShape::getAABB);
        ClassDB::bind_method(D_METHOD("get_vox_triangulation_round"), &VoxelShape::getTriangulationRound);
        ClassDB::bind_method(D_METHOD("get_vox_triangulation_blocky"), &VoxelShape::getTriangulationBlocky);
        ClassDB::bind_method(D_METHOD("save_vox_triangulation", "filename"), &VoxelShape::saveTriangulation);
        ClassDB::bind_method(D_METHOD("get_vox_val", "index"), &VoxelShape::get);
        ClassDB::bind_method(D_METHOD("set_vox_val", "index", "value"), &VoxelShape::set);
        ClassDB::bind_method(D_METHOD("get_vox_resolution"), &VoxelShape::getResolution);
        ClassDB::bind_method(D_METHOD("is_vox_inside", "index"), &VoxelShape::isInside);
    }

    VoxelShape::VoxelShape()
    {
        shape = std::make_unique<Library::BinaryVolume>();
    }

    VoxelShape::~VoxelShape() {}

    godot::String VoxelShape::_to_string() const
    {
        std::string result = "{ Voxel: " + std::to_string(shape->dimension.x) + "/" + std::to_string(shape->dimension.y) + "/" + std::to_string(shape->dimension.z) + "}";
        return result.c_str();
    }

    Ref<VoxelShape> VoxelShape::loadVoxFromFile(const godot::String& str)
    {
        std::string                            filename = std::string(str.utf8());
        std::shared_ptr<Library::BinaryVolume> result   = Library::LoadVoxelOperation::loadMagicaVox(filename);
        if (result)
        {
            auto s = godot::Ref<godot::VoxelShape>();
            s.instantiate();
            s->setData(result);
            return s;
        }
        return Ref<VoxelShape>();
    }

    bool VoxelShape::saveVoxToFile(const godot::String& str)
    {
        std::string filename = std::string(str.utf8());
        Library::SaveVoxelOperation::saveMagicaVox(*shape, filename);
        return true;
    }

    void VoxelShape::setData(std::shared_ptr<Library::BinaryVolume> vol)
    {
        shape = vol;
    }

    Library::BinaryVolume& VoxelShape::getData()
    {
        return *shape;
    }

    Library::BinaryVolume& VoxelShape::getData() const
    {
        return *shape;
    }

    godot::AABB VoxelShape::getAABB() const
    {
        Vector3 position = Vector3(getData().origin.x, getData().origin.y, getData().origin.z);
        Vector3 size     = Vector3(getData().size.x, getData().size.y, getData().size.z);
        return AABB(position, size);
    }

    Ref<ArrayMesh> VoxelShape::getTriangulationRound() const
    {
        auto mesh = Library::TriangulateOperation::triangulateRound(getData());
        if (!mesh)
            return nullptr;

        Ref<SurfaceTool> st;
        st.instantiate();
        st->begin(Mesh::PRIMITIVE_TRIANGLES);

        for (size_t i = 0; i < mesh->vertices.size(); ++i)
        {
            auto v = Vector3((real_t)mesh->vertices[i].x, (real_t)mesh->vertices[i].y, (real_t)mesh->vertices[i].z);
            st->add_vertex(v);
        }
        PackedInt32Array godot_indices;
        godot_indices.resize(mesh->indices.size());
        for (size_t i = 0; i < mesh->indices.size(); i++)
        {
            st->add_index(mesh->indices[i]);
        }
        st->generate_normals();
        Ref<ArrayMesh> result = st->commit();
        return result;
    }

    Ref<ArrayMesh> VoxelShape::getTriangulationBlocky() const
    {
        auto mesh = Library::TriangulateOperation::triangulateBlocky(getData());
        if (!mesh)
            return nullptr;

        Ref<SurfaceTool> st;
        st.instantiate();
        st->begin(Mesh::PRIMITIVE_TRIANGLES);

        for (size_t i = 0; i < mesh->vertices.size(); ++i)
        {
            auto v = Vector3((real_t)mesh->vertices[i].x, (real_t)mesh->vertices[i].y, (real_t)mesh->vertices[i].z);
            st->add_vertex(v);
        }
        PackedInt32Array godot_indices;
        godot_indices.resize(mesh->indices.size());
        for (size_t i = 0; i < mesh->indices.size(); i++)
        {
            st->add_index(mesh->indices[i]);
        }
        st->generate_normals();
        Ref<ArrayMesh> result = st->commit();
        return result;
    }

    void VoxelShape::saveTriangulation(const godot::String& filename) const
    {
        auto mesh = Library::TriangulateOperation::triangulateRound(getData());
        mesh->saveAsSTL(std::string(filename.utf8()));
    }

    godot::Vector3i VoxelShape::getResolution() const
    {
        return godot::Vector3i(shape->dimension.x, shape->dimension.y, shape->dimension.z);
    }

    bool VoxelShape::get(const godot::Vector3i& p) const
    {
        if (isInside(p))
            return shape->data[p.x + p.y * shape->dimension.x + p.z * shape->dimension.x * shape->dimension.y];
    }

    void VoxelShape::set(const godot::Vector3i& p, bool val)
    {
        if (isInside(p))
            shape->data[p.x + p.y * shape->dimension.x + p.z * shape->dimension.x * shape->dimension.y] = val;
    }

    bool VoxelShape::isInside(const godot::Vector3i& p) const
    {
        return p.x >= 0 && p.y >= 0 && p.z >= 0 && p.x < shape->dimension.x && p.y < shape->dimension.y && p.z < shape->dimension.z;
    }
}