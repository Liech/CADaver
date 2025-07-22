#include "CADShell.h"

#include "CADFace.h"
#include "TopAbs2String.h"

#include <BRepTools_ShapeSet.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>

namespace Library
{
    CADShell::CADShell() {}
    CADShell::~CADShell() {}

    std::string CADShell::toString() const
    {
        std::stringstream  ss;
        BRepTools_ShapeSet shapeSet;
        shapeSet.Add(getData());
        ss << "Shell (" << shapeSet.NbShapes() << " face)";
        std::string result = ss.str();
        return result;
    }

    std::string CADShell::getType() const
    {
        return TopAbs2String(TopAbs_SHELL);
    }

    TopoDS_Shell& CADShell::get()
    {
        return (TopoDS_Shell&)getData();
    }

    const TopoDS_Shell& CADShell::get() const
    {
        return (const TopoDS_Shell&)getData();
    }

    std::vector<std::unique_ptr<CADFace>> CADShell::getFaces() const
    {
        std::vector<std::unique_ptr<CADFace>> result;
        TopExp_Explorer                       faceExplorer(get(), TopAbs_FACE);
        while (faceExplorer.More())
        {
            auto sub = std::make_unique<CADFace>();
            sub->setData(TopoDS::Face(faceExplorer.Current()));
            result.push_back(std::move(sub));
            faceExplorer.Next();
        }
        return result;
    }

}