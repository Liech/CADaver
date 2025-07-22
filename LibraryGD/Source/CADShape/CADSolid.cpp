#include "CADSolid.h"
#include "CADShell.h"
#include "Library/CAD/CADShell.h"
#include "Library/CAD/CADSolid.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
    void CADSolid::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_cad_shells"), &CADSolid::getShells);
        ClassDB::bind_method(D_METHOD("get_cad_name"), &CADSolid::getName);
        ClassDB::bind_method(D_METHOD("set_cad_name"), &CADSolid::setName);
    }

    CADSolid::CADSolid() {}
    CADSolid::~CADSolid() {}

    const Library::CADSolid& CADSolid::get() const
    {
        return (const Library::CADSolid&)getData();
    }

    Library::CADSolid& CADSolid::get()
    {
        return (Library::CADSolid&)getData();
    }

    TypedArray<godot::CADShell> CADSolid::getShells() const
    {
        TypedArray<godot::CADShell> result;
        for (auto& x : get().getShells())
        {
            Ref<godot::CADShell> sub;
            sub.instantiate();
            sub->setData(std::move(x));
            result.push_back(sub);
        }
        return result;
    }

    godot::String CADSolid::getName() const
    {
        return get().getName().c_str();
    }

    void CADSolid::setName(godot::String name)
    {
        get().setName(std::string(name.utf8()));
    }
}