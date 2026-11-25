#pragma once

#include "CADacomb/VectorGraphics/Data/VectorGraphics.h"
#include "Database/Asset.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

// Headless data asset holding a 2D VectorGraphics document placed in 3D space via its Asset transformation.
class VectorGraphicsAsset : public Asset
{
  public:
    VectorGraphicsAsset();
    virtual ~VectorGraphicsAsset();

    std::string getType() const override;

    const CADacomb::VectorGraphics::VectorGraphics& getVectorGraphics() const;
    CADacomb::VectorGraphics::VectorGraphics&       getVectorGraphics();
    void                                            setVectorGraphics(std::shared_ptr<CADacomb::VectorGraphics::VectorGraphics> document);

    const glm::dvec2& getWorldSize() const;
    void              setWorldSize(const glm::dvec2& size);

    size_t getSizeInBytes() const override;

  private:
    std::shared_ptr<CADacomb::VectorGraphics::VectorGraphics> vectorGraphics;
    glm::dvec2                                                worldSize = glm::dvec2(1.0, 1.0);
};
