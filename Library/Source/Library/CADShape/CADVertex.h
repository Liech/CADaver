#pragma once

#include "CADShape.h"

class TopoDS_Vertex;

namespace Library
{
    class CADVertex : public CADShape
    {
      public:
        CADVertex();
        virtual ~CADVertex();

        virtual std::string toString() const override;
        virtual std::string getType() const override;

        TopoDS_Vertex&       get();
        const TopoDS_Vertex& get() const;

      private:
    };
}