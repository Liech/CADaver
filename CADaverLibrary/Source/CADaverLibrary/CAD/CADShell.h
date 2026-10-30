#pragma once

#include "CADShape.h"

class TopoDS_Shell;

namespace Library
{
    class CADFace;

    class CADShell : public CADShape
    {
      public:
        CADShell();
        virtual ~CADShell();

        virtual std::string toString() const override;
        virtual std::string getType() const override;

        TopoDS_Shell&       get();
        const TopoDS_Shell& get() const;  
        
        std::vector<std::unique_ptr<CADFace>> getFaces() const;

      private:
    };
}