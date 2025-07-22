#include "CADVertex.h"
#include "Library/CAD/CADVertex.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
    void CADVertex::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_cad_position"), &CADVertex::getPosition);
    }

    CADVertex::CADVertex() {}
    CADVertex::~CADVertex() {}

    const Library::CADVertex& CADVertex::get() const
    {
        return (const Library::CADVertex&)getData();
    }
    Library::CADVertex& CADVertex::get()
    {
        return (Library::CADVertex&)getData();
    }

    Vector3 CADVertex::getPosition() const
    {
        glm::dvec3 pos = get().getPosition();
        return Vector3(pos.x, pos.y, pos.z);
    }
}