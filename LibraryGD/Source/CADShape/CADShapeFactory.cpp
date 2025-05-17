#include "CADShapeFactory.h"

#include "CADCompSolid.h"
#include "CADCompound.h"
#include "CADEdge.h"
#include "CADFace.h"
#include "CADShell.h"
#include "CADSolid.h"
#include "CADVertex.h"
#include "CADWire.h"
#include "Library/CADShape/CADCompSolid.h"
#include "Library/CADShape/CADCompound.h"
#include "Library/CADShape/CADEdge.h"
#include "Library/CADShape/CADFace.h"
#include "Library/CADShape/CADShell.h"
#include "Library/CADShape/CADSolid.h"
#include "Library/CADShape/CADVertex.h"
#include "Library/CADShape/CADWire.h"

namespace godot
{
    godot::Variant CADShapeFactory::make(std::shared_ptr<Library::CADShape> data)
    {
        auto type = data->getType();

        if (type == "TopAbs_COMPOUND")
        {
            auto result = godot::Ref<godot::CADCompound>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_COMPSOLID")
        {
            auto result = godot::Ref<godot::CADCompSolid>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_SOLID")
        {
            auto result = godot::Ref<godot::CADSolid>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_SHELL")
        {
            auto result = godot::Ref<godot::CADShell>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_FACE")
        {
            auto result = godot::Ref<godot::CADFace>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_WIRE")
        {
            auto result = godot::Ref<godot::CADWire>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_EDGE")
        {
            auto result = godot::Ref<godot::CADEdge>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else if (type == "TopAbs_VERTEX")
        {
            auto result = godot::Ref<godot::CADVertex>();
            result.instantiate();
            result->setData(data);
            return result;
        }
        else
        {
            auto result = godot::Ref<godot::CADShape>();
            result.instantiate();
            result->setData(data);
            return result;
        }
    }
}