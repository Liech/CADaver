#pragma once

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Asset.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

// Headless data asset containing triangulation geometry and visual properties.
class TriangulationAsset : public Asset
{
  public:
    TriangulationAsset();
    virtual ~TriangulationAsset();

    std::string getType() const override;

    const CADacomb::Triangulation& getTriangulation() const;
    CADacomb::Triangulation&       getTriangulation();
    void                           setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri);

    const glm::vec3& getMeshColor() const;
    void             setMeshColor(const glm::vec3& color);

    size_t getSizeInBytes() const override;

  private:
    std::unique_ptr<CADacomb::Triangulation> triangulation;
    glm::vec3                                meshColor = glm::vec3(0.75f, 0.78f, 0.82f);
};
