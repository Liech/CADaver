#include "TriangleShape.h"

#include "CADShape/CADShape.h"
#include "CADShape/CADShapeFactory.h"
#include "Library/CAD/CADShape.h"
#include "Library/Operation/IO/LoadCADOperation.h"
#include "Library/Operation/IO/LoadVoxelOperation.h"
#include "Library/Triangle/Clustering.h"
#include "Library/Triangle/HalfEdge/HalfEdge.h"
#include "Library/Triangle/HalfEdge/HalfEdgeHealth.h"
#include "Library/Triangle/HalfEdge/mesh2halfedge.h"
#include "Library/Triangle/Triangulation.h"
#include "Library/Voxel/BinaryVolume.h"
#include "VoxelShape.h"
#include <godot_cpp/classes/surface_tool.hpp>

namespace godot
{
    void TriangleShape::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("save_tri_to_file", "filename"), &TriangleShape::saveTriToFile);
        ClassDB::bind_static_method("TriangleShape", D_METHOD("load_tri_from_file", "filename"), &TriangleShape::loadTriFromFile);
        ClassDB::bind_method(D_METHOD("get_array_mesh"), &TriangleShape::getMesh);
        ClassDB::bind_method(D_METHOD("get_tri_aabb"), &TriangleShape::getAABB);
        ClassDB::bind_method(D_METHOD("get_vertex", "index"), &TriangleShape::getVertex);
        ClassDB::bind_method(D_METHOD("to_vox", "resolution"), &TriangleShape::toVoxel);
        ClassDB::bind_method(D_METHOD("to_cad_dumb"), &TriangleShape::toCad_dumb);
        ClassDB::bind_method(D_METHOD("normal_cluster", "grow_func", "allowholes"), &TriangleShape::normal_cluster);
        ClassDB::bind_method(D_METHOD("cluster_border", "cluster"), &TriangleShape::cluster_border);
        ClassDB::bind_method(D_METHOD("get_halfedge_string"), &TriangleShape::toHalfEdgeString);
        ClassDB::bind_method(D_METHOD("get_halfedge_report"), &TriangleShape::getHalfEdgeReport);
        ClassDB::bind_method(D_METHOD("get_mesh_report"), &TriangleShape::getMeshReport);
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
        st->generate_normals(true);
        Ref<ArrayMesh> result = st->commit();
        return result;
    }

    godot::AABB TriangleShape::getAABB() const
    {
        auto aabb = shape->getAABB();
        return AABB(Vector3(aabb.first.x, aabb.first.y, aabb.first.z), Vector3(aabb.second.x, aabb.second.y, aabb.second.z));
    }

    Ref<VoxelShape> TriangleShape::toVoxel(const Vector3i& resolution) const
    {
        std::shared_ptr<Library::BinaryVolume> resultShape = Library::LoadVoxelOperation::voxelize(*shape, glm::ivec3(resolution.x, resolution.y, resolution.z));
        Ref<VoxelShape>                        result;
        result.instantiate();
        result->setData(resultShape);
        return result;
    }

    Ref<CADShape> TriangleShape::toCad_dumb() const
    {
        std::shared_ptr<Library::CADShape> resultShape = Library::LoadCADOperation::cadify_dumb(*shape);
        return CADShapeFactory::make(resultShape);
    }

    Vector3 TriangleShape::getVertex(uint64_t index)
    {
        auto result = shape->vertices[index];
        return Vector3(result.x, result.y, result.z);
    }

    godot::Array TriangleShape::cluster_border(godot::Array input)
    {
        std::vector<size_t> translatedInput;
        translatedInput.reserve(input.size());
        for (int i = 0; i < input.size(); i++)
        {
            translatedInput.push_back(static_cast<size_t>(input[i]));
        }

        auto                             halfedge = Library::mesh2halfedge::convert(*shape);
        std::vector<std::vector<size_t>> preResult;
        preResult = Library::Clustering::findBorders(*halfedge, translatedInput);

        godot::Array result;
        for (const auto& vec : preResult)
        {
            godot::Array sub_array;
            sub_array.resize(vec.size());
            for (size_t i = 0; i < vec.size(); i++)
            {
                sub_array[i] = static_cast<int64_t>(vec[i]);
            }
            result.append(sub_array);
        }
        return result;
    }

    godot::Array TriangleShape::normal_cluster(godot::Callable grow_func, bool holes)
    {
        auto wrapper = [&grow_func, this](size_t current, size_t candidate, const Library::Triangulation& t) -> bool
        {
            auto           normCurrent   = t.getFaceNormal(current);
            auto           normCandidate = t.getFaceNormal(candidate);
            godot::Variant result        = grow_func.call(Vector3(normCurrent.x, normCurrent.y, normCurrent.z), Vector3(normCandidate.x, normCandidate.y, normCandidate.z));
            return (bool)result;
        };
        std::vector<std::vector<size_t>> internal_result;
        if (holes)
            internal_result = Library::Clustering::cluster(*shape, wrapper);
        else
            internal_result = Library::Clustering::cluster_withoutHoles(*shape, wrapper);

        godot::Array final_array;
        for (const auto& patch : internal_result)
        {
            godot::PackedInt64Array godot_patch;
            godot_patch.resize(patch.size());

            for (size_t i = 0; i < patch.size(); ++i)
            {
                godot_patch[i] = static_cast<int64_t>(patch[i]);
            }

            final_array.append(godot_patch);
        }

        return final_array;
    }

    godot::String TriangleShape::toHalfEdgeString() const
    {
        std::string result;
        auto        halfedge = Library::mesh2halfedge::convert(*shape);
        result               = Library::HalfEdgeHealth::toString(*halfedge);
        return result.c_str();
    }

    godot::String TriangleShape::getHalfEdgeReport() const
    {
        std::string result;
        auto        halfedge = Library::mesh2halfedge::convert(*shape);
        result               = Library::HalfEdgeHealth::createReport(*halfedge);
        return result.c_str();
    }
    godot::String TriangleShape::getMeshReport() const
    {
        std::string result;
        result = Library::HalfEdgeHealth::createReport(*shape);
        return result.c_str();
    }
}