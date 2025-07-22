#include "CADFace.h"

#include <godot_cpp/core/class_db.hpp>

#include "Library/CAD/CADFace.h"
#include "Library/CAD/CADWire.h"
#include "Library/Triangle/Triangulation.h"

#include "CADWire.h"

namespace godot
{
    void CADFace::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_cad_wires"), &CADFace::getWires);
        ClassDB::bind_method(D_METHOD("get_cad_orientation"), &CADFace::getOrientation);
    }

    CADFace::CADFace() {}
    CADFace::~CADFace() {}

    const Library::CADFace& CADFace::get() const
    {
        return (const Library::CADFace&)getData();
    }

    Library::CADFace& CADFace::get()
    {
        return (Library::CADFace&)getData();
    }

    TypedArray<godot::CADWire> CADFace::getWires() const
    {
        TypedArray<godot::CADWire> result;
        for (auto& x : get().getWires())
        {
            Ref<godot::CADWire> sub;
            sub.instantiate();
            sub->setData(std::move(x));
            result.push_back(sub);
        }
        return result;
    }
    
    godot::String CADFace::getOrientation() const
    {
        return godot::String(get().getOrientation().c_str());
    }

}