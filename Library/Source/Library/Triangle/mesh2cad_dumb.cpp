#include "mesh2cad_dumb.h"

#include "CAD/CADShape.h"
#include "CAD/CADShapeFactory.h"
#include "Triangulation.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Builder.hxx>
#include <BRep_TFace.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopAbs_ShapeEnum.hxx> // For TopAbs_SHELL, etc.
#include <TopExp_Explorer.hxx>  // For TopExp_Explorer
#include <TopoDS.hxx>           // For the TopoDS casting utility
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <gp_Pnt.hxx>

namespace Library
{
    std::unique_ptr<CADShape> mesh2cad_dumb::cadify_dumb(const Triangulation& mesh)
    {
        BRepBuilderAPI_Sewing sewer;
        sewer.SetTolerance(1e-6); // Adjust based on your mesh precision

        int numTriangles = static_cast<int>(mesh.indices.size() / 3);

        for (int i = 0; i < numTriangles; ++i)
        {
            int    offset0 = 0;
            int    offset1 = 2;
            int    offset2 = 1;
            gp_Pnt p1(mesh.vertices[mesh.indices[i * 3 + offset0]].x, mesh.vertices[mesh.indices[i * 3 + offset0]].y, mesh.vertices[mesh.indices[i * 3 + offset0]].z);
            gp_Pnt p2(mesh.vertices[mesh.indices[i * 3 + offset1]].x, mesh.vertices[mesh.indices[i * 3 + offset1]].y, mesh.vertices[mesh.indices[i * 3 + offset1]].z);
            gp_Pnt p3(mesh.vertices[mesh.indices[i * 3 + offset2]].x, mesh.vertices[mesh.indices[i * 3 + offset2]].y, mesh.vertices[mesh.indices[i * 3 + offset2]].z);

            // 1. Create Edges
            TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(p1, p2);
            TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(p2, p3);
            TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(p3, p1);

            // 2. Create Wire
            BRepBuilderAPI_MakeWire mw(e1, e2, e3);
            if (!mw.IsDone())
                continue;
            TopoDS_Wire wire = mw.Wire();

            // 3. Create Face
            // This uses the "BRepBuilderAPI_MakeFace(const TopoDS_Wire& W, OnlyPlane=false)"
            // constructor from your header. It will find the plane for the triangle.
            BRepBuilderAPI_MakeFace mf(wire, Standard_True);

            if (mf.IsDone())
            {
                sewer.Add(mf.Face());
            }
        }

        sewer.Perform();
        TopoDS_Shape sewedShape = sewer.SewedShape();

        // Check if we have a shell
        if (sewedShape.ShapeType() == TopAbs_SHELL || sewedShape.ShapeType() == TopAbs_COMPOUND)
        {
            // Convert Shell to Solid
            BRepBuilderAPI_MakeSolid solidMaker;

            // If the mesh is a single closed manifold, the sewed shape
            // should contain one or more shells.
            for (TopExp_Explorer exp(sewedShape, TopAbs_SHELL); exp.More(); exp.Next())
            {
                solidMaker.Add(TopoDS::Shell(exp.Current()));
            }

            if (solidMaker.IsDone())
            {
                auto result = CADShapeFactory::make(solidMaker.Solid());
                CADShapeFactory::recurseFillChildShapes(*result);
                return result;
            }
        }

        return nullptr;
    }
}