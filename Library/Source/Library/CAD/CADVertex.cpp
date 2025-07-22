#include "CADVertex.h"

#include "TopAbs2String.h"

#include <BRepTools_ShapeSet.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

namespace Library
{
    CADVertex::CADVertex() {}
    CADVertex::~CADVertex() {}

    std::string CADVertex::toString() const
    {
        std::stringstream ss;
        gp_Pnt            p = BRep_Tool::Pnt(TopoDS::Vertex(getData()));
        ss << "Vertex (" << std::fixed << std::setprecision(1) << p.X() << "," << std::fixed << std::setprecision(1) << p.Y() << "," << std::fixed << std::setprecision(1) << p.Z() << ")";
        std::string result = ss.str();
        return result;
    }

    std::string CADVertex::getType() const
    {
        return TopAbs2String(TopAbs_VERTEX);
    }

    TopoDS_Vertex& CADVertex::get()
    {
        return (TopoDS_Vertex&)getData();
    }

    const TopoDS_Vertex& CADVertex::get() const
    {
        return (const TopoDS_Vertex&)getData();
    }

    glm::dvec3 CADVertex::getPosition() const
    {
        gp_Pnt point = BRep_Tool::Pnt(get());
        return glm::dvec3(point.X(),point.Y(),point.Z());
    }
}