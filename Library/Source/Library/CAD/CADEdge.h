#pragma once

#include "CADShape.h"
#include <glm/glm.hpp>
#include <optional>

class TopoDS_Edge;

namespace Library
{
    class CADVertex;

    class CADEdge : public CADShape
    {
      public:
        CADEdge();
        virtual ~CADEdge();

        virtual std::string toString() const override;
        virtual std::string getType() const override;

        TopoDS_Edge&       get();
        const TopoDS_Edge& get() const;

        std::unique_ptr<CADVertex> getStart() const;
        std::unique_ptr<CADVertex> getEnd() const;
        std::string                getOrientation() const;

      private:
    };
}