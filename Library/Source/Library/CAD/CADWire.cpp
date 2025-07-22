#include "CADWire.h"

#include "CADEdge.h"
#include "TopAbs2String.h"

#include <BRepTools_ShapeSet.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>

namespace Library
{
    CADWire::CADWire() {}
    CADWire::~CADWire() {}

    std::string CADWire::toString() const
    {
        std::stringstream ss;
        ss << "Wire";
        bool isClosed = BRep_Tool::IsClosed(TopoDS::Wire(getData()));
        if (isClosed)
            ss << ", C";
        std::string result = ss.str();
        return result;
    }

    std::string CADWire::getType() const
    {
        return TopAbs2String(TopAbs_WIRE);
    }

    TopoDS_Wire& CADWire::get()
    {
        return (TopoDS_Wire&)getData();
    }

    const TopoDS_Wire& CADWire::get() const
    {
        return (const TopoDS_Wire&)getData();
    }

    std::vector<std::unique_ptr<CADEdge>> CADWire::getEdges() const {
        std::vector<std::unique_ptr<CADEdge>> result;
        TopExp_Explorer edgeExplorer(get(), TopAbs_EDGE);
        for (; edgeExplorer.More(); edgeExplorer.Next())
        {
            TopoDS_Edge edge = TopoDS::Edge(edgeExplorer.Current());
            auto        sub  = std::make_unique<CADEdge>();
            sub->setData(edge);
            result.push_back(std::move(sub));
        }
        return result;
    }

    bool CADWire::isClosed() const {
        bool isClosed = BRep_Tool::IsClosed(TopoDS::Wire(getData()));
        return isClosed;
    }
}