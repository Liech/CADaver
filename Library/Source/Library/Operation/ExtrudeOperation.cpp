#include "ExtrudeOperation.h"

#include "CAD/CADShapeFactory.h"
#include "CAD/CADShape.h"

#include <BRepPrimAPI_MakePrism.hxx>
#include <BRep_Builder.hxx>

namespace Library
{
    std::unique_ptr<CADShape> ExtrudeOperation::extrude(const CADShape& input, const glm::dvec3& direction)
    {
        BRepPrimAPI_MakePrism prismMaker(input.getData(), gp_Vec(direction.x, direction.y, direction.z),true); // true is only useful if its not merged into the input
        TopoDS_Shape          extrudedShape = prismMaker.Shape();
        if (extrudedShape.IsNull())
            return nullptr;

        return CADShapeFactory::make(extrudedShape);
    }
}