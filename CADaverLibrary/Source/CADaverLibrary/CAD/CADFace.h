#pragma once

#include "CADShape.h"

class TopoDS_Face;

namespace Library
{
    class CADWire;
    class Triangulation;

    class CADFace : public CADShape
    {
      public:
        CADFace();
        virtual ~CADFace();

        virtual std::string toString() const override;
        virtual std::string getType() const override;

        TopoDS_Face&       get();
        const TopoDS_Face& get() const;

        std::vector<std::unique_ptr<CADWire>> getWires() const;
        std::string                           getOrientation() const;

      private:
    };
}