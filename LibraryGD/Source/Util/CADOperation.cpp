#include "CADOperation.h"

#include "CADShape/CADShapeFactory.h"
#include "Library/Operation/ExtrudeOperation.h"
#include "Library/CAD/CADShape.h"

namespace godot
{
    void CADOperation::_bind_methods()
    {
        ClassDB::bind_static_method("CADOperation", D_METHOD("extrude", "cadshape", "direction"), &CADOperation::extrude);
    }

    CADOperation::CADOperation() {}

    CADOperation::~CADOperation() {}

    Ref<godot::CADShape> CADOperation::extrude(Ref<godot::CADShape> shape, Vector3 direction)
    {
        auto extrusion = Library::ExtrudeOperation::extrude(shape->getData(), glm::dvec3(direction.x, direction.y, direction.z));
        if (!extrusion)
            return nullptr;
        return godot::CADShapeFactory::make(std::move(extrusion));
    }
}