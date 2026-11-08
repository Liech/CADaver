#pragma once

#include "Concept/Concept.h"

#include <memory>
#include <string>
#include <vector>

// Factory providing access to all registered conceptual documentation items.
class ConceptFactory
{
  public:
    ConceptFactory();
    virtual ~ConceptFactory();

    std::shared_ptr<Concept>              create(const std::string& type) const;
    std::vector<std::shared_ptr<Concept>> getAllConcepts() const;

  private:
    void registerConcepts();

    std::vector<std::shared_ptr<Concept>> concepts;
};
