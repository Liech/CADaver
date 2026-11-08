#pragma once

#include "Concept/Concept.h"

// Concept explaining the Stereolithography (STL) triangle surface format.
class StlFileConcept : public Concept
{
  public:
    StlFileConcept();
    virtual ~StlFileConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
