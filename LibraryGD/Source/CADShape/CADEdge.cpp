#include "CADEdge.h"
#include "CADVertex.h"
#include "Library/CAD/CADEdge.h"
#include "Library/CAD/CADVertex.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
    void CADEdge::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_cad_start"), &CADEdge::getCADStart);
        ClassDB::bind_method(D_METHOD("get_cad_end"), &CADEdge::getCADEnd);
        ClassDB::bind_method(D_METHOD("get_cad_orientation"), &CADEdge::getCADOrientation);
    }

    CADEdge::CADEdge() {}
    CADEdge::~CADEdge() {}

    const Library::CADEdge& CADEdge::get() const
    {
        return (const Library::CADEdge&)getData();
    }
    Library::CADEdge& CADEdge::get()
    {
        return (Library::CADEdge&)getData();
    }

    Ref<godot::CADVertex> CADEdge::getCADStart() const
    {
        Ref<godot::CADVertex> result;
        auto                  d = get().getStart();
        if (d)
        {
            result.instantiate();
            result->setData(std::move(d));
            return result;
        }
        return result;
    }

    Ref<godot::CADVertex> CADEdge::getCADEnd() const
    {
        Ref<godot::CADVertex> result;
        auto                  d = get().getEnd();
        if (d)
        {
            result.instantiate();
            result->setData(std::move(d));
            return result;
        }
        return result;
    }

    godot::String CADEdge::getCADOrientation() const
    {
        return get().getOrientation().c_str();
    }

}