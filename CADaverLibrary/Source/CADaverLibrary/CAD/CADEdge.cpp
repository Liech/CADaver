#include "CADEdge.h"

#include "TopAbs2String.h"

#include "CADVertex.h"
#include "TopAbs2String.h"
#include <BRepTools_ShapeSet.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopoDS_Edge.hxx>

namespace Library
{
    CADEdge::CADEdge() {}
    CADEdge::~CADEdge() {}

    std::string CADEdge::toString() const
    {
        return getType();
    }

    std::string CADEdge::getType() const
    {
        return TopAbs2String(TopAbs_EDGE);
    }

    TopoDS_Edge& CADEdge::get()
    {
        return (TopoDS_Edge&)getData();
    }

    const TopoDS_Edge& CADEdge::get() const
    {
        return (const TopoDS_Edge&)getData();
    }

    std::unique_ptr<CADVertex> CADEdge::getStart() const
    {
        TopoDS_Vertex firstVertex = TopExp::FirstVertex(get(), true);
        if (!firstVertex.IsNull())
        {
            auto result = std::make_unique<CADVertex>();
            result->setData(firstVertex);
            return std::move(result);
        }
        else
        {
            return nullptr;
        }
    }

    std::unique_ptr<CADVertex> CADEdge::getEnd() const
    {
        TopoDS_Vertex firstVertex = TopExp::LastVertex(get(), true);
        if (!firstVertex.IsNull())
        {
            auto result = std::make_unique<CADVertex>();
            result->setData(firstVertex);
            return std::move(result);
        }
        else
        {
            return nullptr;
        }
    }

    std::string CADEdge::getOrientation() const
    {
        TopAbs_Orientation orientation = get().Orientation();
        return TopAbsOrientation2String(orientation);
    }

}