#include "CADCompound.h"

#include "TopAbs2String.h"

#include <BRepTools_ShapeSet.hxx>
#include <TopoDS_Compound.hxx>

namespace Library
{
    CADCompound::CADCompound() {}

    CADCompound::~CADCompound() {}

    std::string CADCompound::getType() const
    {
        return TopAbs2String(TopAbs_COMPOUND);
    }

    std::string CADCompound::toString() const
    {
        std::stringstream  ss;
        BRepTools_ShapeSet shapeSet;
        shapeSet.Add(getData());
        ss << "Compound (" << shapeSet.NbShapes() << " sub)";
        std::string result = ss.str();
        return result;
    }

    TopoDS_Compound& CADCompound::get()
    {
        return (TopoDS_Compound&)getData();
    }

    const TopoDS_Compound& CADCompound::get() const
    {
        return (const TopoDS_Compound&)getData();
    }
}