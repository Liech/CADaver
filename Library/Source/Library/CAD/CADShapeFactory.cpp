#include "CADShapeFactory.h"

#include "CADCompSolid.h"
#include "CADCompound.h"
#include "CADEdge.h"
#include "CADFace.h"
#include "CADShell.h"
#include "CADSolid.h"
#include "CADVertex.h"
#include "CADWire.h"

#include <STEPControl_Reader.hxx>
#include <StepShape_ManifoldSolidBrep.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

namespace Library
{

    std::vector<std::unique_ptr<CADShape>> CADShapeFactory::generateChildren(CADShape& shape)
    {
        std::vector<std::unique_ptr<CADShape>> result;
        TopoDS_Iterator                        it(shape.getData(), true, false);

        while (it.More())
        {
            auto current = CADShapeFactory::make(it.Value());
            it.Next();
            result.push_back(std::move(current));
        }
        return result;
    }

    void CADShapeFactory::fillSolidName(CADSolid& shape, XSControl_WorkSession& ws)
    {
        Handle(Standard_Transient) entity = ws.TransferReader()->EntityFromShapeResult(shape.getData(), 1);

        if (!entity.IsNull())
        {
            Handle(StepShape_ManifoldSolidBrep) brep = Handle(StepShape_ManifoldSolidBrep)::DownCast(entity);
            if (!brep.IsNull())
            {
                Handle(TCollection_HAsciiString) brepName = brep->Name();
                if (!brepName.IsNull())
                {
                    ((CADSolid&)shape).setName(brepName->ToCString());
                }
            }
        }
    }

    void CADShapeFactory::recurseFillChildShapes(CADShape& shape, XSControl_WorkSession& ws)
    {
        if (shape.getType() == "TopAbs_SOLID")
            fillSolidName((CADSolid&)shape, ws);

        for (auto& x : generateChildren(shape))
        {
            recurseFillChildShapes(*x, ws);
            shape.addChild(std::move(x));
        }
    }

    std::unique_ptr<CADShape> CADShapeFactory::make(const std::string& filename)
    {
        STEPControl_Reader    reader;
        IFSelect_ReturnStatus status = reader.ReadFile(filename.c_str());
        if (status != IFSelect_RetDone)
            return nullptr;

        Standard_Integer nRoots = reader.NbRootsForTransfer();
        for (Standard_Integer i = 1; i <= nRoots; i++)
        {
            reader.TransferRoot(i);
        }
        TopoDS_Shape mainShape = reader.OneShape();
        if (mainShape.IsNull())
            return nullptr;
        Handle(XSControl_WorkSession) ws = reader.WS();

        auto result = make(mainShape);
        recurseFillChildShapes(*result, *ws);
        return std::move(result);
    }

    std::unique_ptr<CADShape> CADShapeFactory::makeSimple(const std::string& filename)
    {
        STEPControl_Reader    reader;
        IFSelect_ReturnStatus status = reader.ReadFile(filename.c_str());

        if (status != IFSelect_RetDone)
            return nullptr;
        reader.TransferRoots();
        return make(reader.OneShape());
    }

    std::unique_ptr<CADShape> CADShapeFactory::make(int t)
    {
        TopAbs_ShapeEnum          type = (TopAbs_ShapeEnum)t;
        std::unique_ptr<CADShape> result;
        switch (type)
        {
            case TopAbs_COMPOUND:
                result = std::make_unique<CADCompound>();
                break;
            case TopAbs_COMPSOLID:
                result = std::make_unique<CADCompSolid>();
                break;
            case TopAbs_SOLID:
                result = std::make_unique<CADSolid>();
                break;
            case TopAbs_SHELL:
                result = std::make_unique<CADShell>();
                break;
            case TopAbs_FACE:
                result = std::make_unique<CADFace>();
                break;
            case TopAbs_WIRE:
                result = std::make_unique<CADWire>();
                break;
            case TopAbs_EDGE:
                result = std::make_unique<CADEdge>();
                break;
            case TopAbs_VERTEX:
                result = std::make_unique<CADVertex>();
                break;
            case TopAbs_SHAPE:
                result = std::make_unique<CADShape>();
                break;
            default:
                return nullptr;
        }
        return std::move(result);
    }

    std::unique_ptr<CADShape> CADShapeFactory::make(const TopoDS_Shape& data)
    {
        auto result = make(data.ShapeType());
        result->setData(data);
        return std::move(result);
    }

}