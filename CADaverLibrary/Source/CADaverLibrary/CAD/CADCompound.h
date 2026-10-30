#pragma once

#include <string>
#include <memory>
#include <vector>

#include "CADShape.h"

class TopoDS_Compound;

namespace Library
{
  class CADCompound : public CADShape
  {
    public:
      CADCompound();
      virtual ~CADCompound();

      virtual std::string toString() const override;
      virtual std::string getType() const  override;

      TopoDS_Compound&       get();
      const TopoDS_Compound& get() const;
    private:

  };
}