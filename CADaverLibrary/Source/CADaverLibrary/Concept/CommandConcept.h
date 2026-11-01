#pragma once

#include "Concept/Concept.h"

// Concept explaining the Command pattern, headless execution, and transactionality.
class CommandConcept : public Concept
{
  public:
    CommandConcept();
    virtual ~CommandConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
