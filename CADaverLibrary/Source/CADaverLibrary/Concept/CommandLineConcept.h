#pragma once

#include "Concept/Concept.h"

// Concept documenting command line arguments for scripting, headless execution, and MCP server startup.
class CommandLineConcept : public Concept
{
  public:
    CommandLineConcept();
    virtual ~CommandLineConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
