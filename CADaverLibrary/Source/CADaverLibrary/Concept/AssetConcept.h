#pragma once

#include "Concept/Concept.h"

// Concept explaining Project Assets, domain entity lifecycles, and headless decoupling.
class AssetConcept : public Concept
{
  public:
    AssetConcept();
    virtual ~AssetConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
