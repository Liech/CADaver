#include "CADShell.h"

#include "CADFace.h"
#include "Library/CAD/CADShell.h"
#include "Library/CAD/CADFace.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
    void CADShell::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_cad_faces"), &CADShell::getFaces);
    }

    CADShell::CADShell() {}
    CADShell::~CADShell() {}

    const Library::CADShell& CADShell::get() const
    {
        return (const Library::CADShell&)getData();
    }

    Library::CADShell& CADShell::get()
    {
        return (Library::CADShell&)getData();
    }

    TypedArray<godot::CADFace> CADShell::getFaces() const
    {
        TypedArray<godot::CADFace> result;
        for (auto& x : get().getFaces())
        {
            Ref<godot::CADFace> sub;
            sub.instantiate();
            sub->setData(std::move(x));
            result.push_back(sub);
        }
        return result;
    }
}