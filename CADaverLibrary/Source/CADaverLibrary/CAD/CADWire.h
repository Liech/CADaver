#pragma once

#include <vector>

#include "CADShape.h"

class TopoDS_Wire;
class TopoDS_Edge;

namespace Library
{
    class CADEdge;

    class CADWire : public CADShape
    {
      public:
        CADWire();
        virtual ~CADWire();

        virtual std::string toString() const override;
        virtual std::string getType() const override;

        TopoDS_Wire&       get();
        const TopoDS_Wire& get() const;

        std::vector<std::unique_ptr<CADEdge>> getEdges() const;
        bool                                  isClosed() const;

      private:
    };
}