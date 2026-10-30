#include "CADSolid.h"

#include "CADShell.h"
#include "TopAbs2String.h"

#include <BRepGProp.hxx>
#include <BRepTools_ShapeSet.hxx>
#include <GProp_GProps.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>

namespace Library
{
    CADSolid::CADSolid() {}
    CADSolid::~CADSolid() {}

    std::string CADSolid::toString() const
    {
        std::stringstream ss;
        GProp_GProps      props;
        BRepGProp::VolumeProperties(TopoDS::Solid(getData()), props);
        ss << "Solid, V=" << std::fixed << std::setprecision(2) << props.Mass();
        std::string result = ss.str();
        return result;
    }

    std::string CADSolid::getType() const
    {
        return TopAbs2String(TopAbs_SOLID);
    }

    TopoDS_Solid& CADSolid::get()
    {
        return (TopoDS_Solid&)getData();
    }

    const TopoDS_Solid& CADSolid::get() const
    {
        return (const TopoDS_Solid&)getData();
    }

    std::vector<std::unique_ptr<CADShell>> CADSolid::getShells() const
    {
        std::vector<std::unique_ptr<CADShell>> result;
        TopExp_Explorer                        shellExplorer(get(), TopAbs_SHELL);
        while (shellExplorer.More())
        {
            TopoDS_Shell currentShell = TopoDS::Shell(shellExplorer.Current());
            auto         sub          = std::make_unique<CADShell>();
            sub->setData(currentShell);
            result.push_back(std::move(sub));
            shellExplorer.Next();
        }
        return result;
    }

    void CADSolid::setName(const std::string& name_)
    {
        name = name_;
    }

    std::string CADSolid::getName() const
    {
        return name;
    }
}