#pragma once

#include "Concept/Concept.h"

// Concept explaining SubAssets and subordinate components owned by Assets.
class SubAssetConcept : public Concept
{
  public:
    SubAssetConcept();
    virtual ~SubAssetConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
