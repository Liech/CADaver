#include "CADShape.h"

#include <stdexcept>

#include "CADShapeFactory.h"
#include "TopAbs2String.h"

#include <Poly_Triangulation.hxx>
#include <STEPControl_Writer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepBndLib.hxx>

namespace Library
{
    CADShape::CADShape()
    {
        data = std::make_unique<TopoDS_Shape>();
    }

    CADShape::~CADShape() {}

    std::string CADShape::toString() const
    {
        return getType();
    }

    std::string CADShape::getType() const
    {
        return TopAbs2String(TopAbs_SHAPE);
    }

    const std::vector<std::shared_ptr<CADShape>>& CADShape::getChildren() const
    {
        return children;
    }
    
    void CADShape::addChild(std::unique_ptr<CADShape> child)
    {
        children.push_back(std::move(child));
    }

    TopoDS_Shape& CADShape::getData()
    {
        return *data;
    }

    const TopoDS_Shape& CADShape::getData() const
    {
        return *data;
    }
    void CADShape::setData(const TopoDS_Shape& d)
    {
        data = std::make_unique<TopoDS_Shape>(d);
    }

    bool CADShape::save(const std::string& filename) const
    {
        // https://dev.opencascade.org/content/export-multi-shapes-single-step-file
        STEPControl_Writer    aWriter;
        IFSelect_ReturnStatus aStat = aWriter.Transfer(*data, STEPControl_AsIs);
        aStat                       = aWriter.Write(filename.c_str());
        if (aStat != IFSelect_RetDone)
            return false;
        return true;
    }

    std::unique_ptr<CADShape> CADShape::load(const std::string& filename)
    {
        std::unique_ptr<CADShape> result = CADShapeFactory::make(filename);
        return std::move(result);
    }

    std::pair<glm::dvec3, glm::dvec3> CADShape::getBoundingBox() const {
        Bnd_Box aBndBox;
        BRepBndLib::Add(*data, aBndBox);
        if (aBndBox.IsVoid())
        {
            return std::make_pair(glm::dvec3(),glm::dvec3());
        }
        std::pair<glm::dvec3, glm::dvec3> result;
        aBndBox.Get(result.first.x, result.first.y, result.first.z, result.second.x, result.second.y, result.second.z);

        result.second = result.second - result.first;
        return result;
    }
}
