#pragma once

#include <memory>
#include <string>
#include <vector>

class TopoDS_Shape;
class XSControl_WorkSession;

namespace Library
{
    class CADShape;
    class CADSolid;

    class CADShapeFactory
    {
      public:
        static std::unique_ptr<CADShape> make(const std::string& filename);
        static std::unique_ptr<CADShape> makeSimple(const std::string& filename);
        static std::unique_ptr<CADShape> make(int type);
        static std::unique_ptr<CADShape> make(const TopoDS_Shape&);

        static std::vector<std::unique_ptr<CADShape>> generateChildren(CADShape& shape);

      private:
        static void recurseFillChildShapes(CADShape&, XSControl_WorkSession&);
        static void fillSolidName(CADSolid&, XSControl_WorkSession&);
    };
}