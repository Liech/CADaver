#include "CADShape.h"
#include "CADShapeFactory.h"
#include "Library/CAD/CADShape.h"
#include "Library/Operation/TriangulateOperation.h"
#include "Library/Triangle/Triangulation.h"
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include "TriangleShape.h"

namespace godot
{
    void CADShape::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("save_cad_to_file", "filename"), &CADShape::saveCadToFile);
        ClassDB::bind_method(D_METHOD("get_cad_children"), &CADShape::getCadChildren);
        ClassDB::bind_method(D_METHOD("get_cad_type"), &CADShape::getCadType);
        ClassDB::bind_method(D_METHOD("get_cad_aabb"), &CADShape::getAABB);

        ClassDB::bind_static_method("CADShape", D_METHOD("load_cad_from_file", "filename"), &CADShape::loadCadFromFile);
        ClassDB::bind_method(D_METHOD("get_cad_triangulation", "precision"), &CADShape::getTriangulation);
    }

    CADShape::CADShape()
    {
        shape = std::make_unique<Library::CADShape>();
    }

    CADShape::~CADShape() {}

    Ref<CADShape> CADShape::loadCadFromFile(const godot::String& str)
    {
        std::string                        filename = std::string(str.utf8());
        std::shared_ptr<Library::CADShape> result   = Library::CADShape::load(filename);
        if (result)
        {
            Ref<CADShape> resultGD = godot::CADShapeFactory::make(result);
            return resultGD;
        }
        return Ref<CADShape>();
    }

    bool CADShape::saveCadToFile(const godot::String& str)
    {
        std::string filename = std::string(str.utf8());
        return shape->save(filename);
    }

    Array CADShape::getCadChildren() const
    {
        Array result;
        for (auto& x : shape->getChildren())
        {
            result.push_back(godot::CADShapeFactory::make(x));
        }
        return result;
    }

    godot::String CADShape::_to_string() const
    {
        return shape->toString().c_str();
    }

    godot::String CADShape::getCadType() const
    {
        return shape->getType().c_str();
    }

    void CADShape::setData(std::shared_ptr<Library::CADShape> data)
    {
        shape = data;
    }

    Library::CADShape& CADShape::getData()
    {
        return *shape;
    }

    const Library::CADShape& CADShape::getData() const
    {
        return *shape;
    }

    godot::AABB CADShape::getAABB() const
    {
        auto    aabb     = getData().getBoundingBox();
        Vector3 position = Vector3(aabb.first.x, aabb.first.y, aabb.first.z);
        Vector3 size     = Vector3(aabb.second.x, aabb.second.y, aabb.second.z);
        return AABB(position, size);
    }

    Ref<TriangleShape> CADShape::getTriangulation(double precision) const
    {
        auto mesh = Library::TriangulateOperation::triangulate(getData(), precision);
        if (!mesh)
            return nullptr;
        Ref<TriangleShape> result;
        result.instantiate();
        result->setData(std::move(mesh));
        return result;
    }
}