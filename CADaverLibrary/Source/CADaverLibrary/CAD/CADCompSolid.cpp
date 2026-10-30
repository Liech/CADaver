#include "CADCompSolid.h"

#include "TopAbs2String.h"
#include "CADSolid.h"

#include <TopAbs_ShapeEnum.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

namespace Library
{
    CADCompSolid::CADCompSolid() {}

    CADCompSolid::~CADCompSolid() {}

    std::string CADCompSolid::getType() const
    {
        return TopAbs2String(TopAbs_COMPSOLID);
    }

    std::string CADCompSolid::toString() const
    {
        std::stringstream  ss;
        BRepTools_ShapeSet shapeSet;
        shapeSet.Add(getData());
        ss << "CompSolid (" << shapeSet.NbShapes() << " solid)";
        std::string result = ss.str();
        return result;
    }

    TopoDS_CompSolid& CADCompSolid::get()
    {
        return (TopoDS_CompSolid&)getData();
    }

    const TopoDS_CompSolid& CADCompSolid::get() const
    {
        return (const TopoDS_CompSolid&)getData();
    }

    std::vector<std::unique_ptr<CADSolid>> CADCompSolid::getSolids() const
    {
        std::vector<std::unique_ptr<CADSolid>> result;
        TopExp_Explorer                       faceExplorer(get(), TopAbs_SOLID);
        while (faceExplorer.More())
        {
            auto sub = std::make_unique<CADSolid>();
            sub->setData(TopoDS::Face(faceExplorer.Current()));
            result.push_back(std::move(sub));
            faceExplorer.Next();
        }
        return result;
    }
}