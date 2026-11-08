#pragma once

#include "Concept/Concept.h"

// Concept explaining Sparse Voxel Octrees (SVO) and volumetric spatial hierarchies.
class SvoFileConcept : public Concept
{
  public:
    SvoFileConcept();
    virtual ~SvoFileConcept();

    std::string getType() const override;
    std::string getDescription() const override;
};
