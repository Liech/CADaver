#pragma once

#include "Concept/Concept.h"

// Concept explaining the STEP (ISO 10303) CAD file format and B-Rep modeling.
class StepFileConcept : public Concept
{
  public:
    StepFileConcept();
    virtual ~StepFileConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
