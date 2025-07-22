#include "CADFace.h"

#include "CADWire.h"
#include "TopAbs2String.h"

#include <BRepGProp.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <GProp_GProps.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>

namespace Library
{
    CADFace::CADFace() {}
    CADFace::~CADFace() {}

    std::string CADFace::toString() const
    {
        std::stringstream ss;
        GProp_GProps      props;
        BRepGProp::SurfaceProperties(TopoDS::Face(getData()), props);
        ss << "Face, A=" << std::fixed << std::setprecision(2) << props.Mass();
        bool isClosed = BRep_Tool::IsClosed(TopoDS::Face(getData()));
        if (isClosed)
            ss << ", C";
        std::string result = ss.str();
        return result;
    }

    std::string CADFace::getType() const
    {
        return TopAbs2String(TopAbs_FACE);
    }

    TopoDS_Face& CADFace::get()
    {
        return (TopoDS_Face&)getData();
    }

    const TopoDS_Face& CADFace::get() const
    {
        return (const TopoDS_Face&)getData();
    }

    std::vector<std::unique_ptr<CADWire>> CADFace::getWires() const
    {
        std::vector<std::unique_ptr<CADWire>> result;
        TopExp_Explorer                       wireExplorer(get(), TopAbs_WIRE);
        while (wireExplorer.More())
        {
            TopoDS_Wire wire = TopoDS::Wire(wireExplorer.Current());
            auto        sub  = std::make_unique<CADWire>();
            sub->setData(wire);
            result.push_back(std::move(sub));
            wireExplorer.Next();
        }
        return result;
    }

    std::string CADFace::getOrientation() const
    {
        return TopAbsOrientation2String(get().Orientation());
    }
}