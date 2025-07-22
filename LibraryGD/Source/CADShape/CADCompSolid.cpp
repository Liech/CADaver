#include "CADCompSolid.h"
#include "CADSolid.h"
#include "Library/CAD/CADSolid.h"
#include "Library/CAD/CADCompSolid.h"
#include <godot_cpp/core/class_db.hpp>

namespace godot
{
    void CADCompSolid::_bind_methods()
    {
        ClassDB::bind_method(D_METHOD("get_cad_solids"), &CADCompSolid::getSolids);
    }

    CADCompSolid::CADCompSolid() {}
    CADCompSolid::~CADCompSolid() {}

    const Library::CADCompSolid& CADCompSolid::get() const
    {
        return (const Library::CADCompSolid&)getData();
    }

    Library::CADCompSolid& CADCompSolid::get()
    {
        return (Library::CADCompSolid&)getData();
    }

    TypedArray<godot::CADSolid> CADCompSolid::getSolids() const
    {
        TypedArray<godot::CADSolid> result;
        for (auto& x : get().getSolids())
        {
            Ref<godot::CADSolid> sub;
            sub.instantiate();
            sub->setData(std::move(x));
            result.push_back(sub);
        }
        return result;
    }
}