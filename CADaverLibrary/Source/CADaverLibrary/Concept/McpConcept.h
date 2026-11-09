#pragma once

#include "Concept/Concept.h"

// Concept explaining Model Context Protocol (MCP) server modes, transports, and exposed tools.
class McpConcept : public Concept
{
  public:
    McpConcept();
    virtual ~McpConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
