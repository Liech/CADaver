#pragma once

#include "Asset.h"
#include "CADacomb/Triangle/Triangulation.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

// Headless data asset containing triangulation geometry and visual properties.
class TriangulationAsset : public Asset
{
  public:
    TriangulationAsset(const std::string& name);
    TriangulationAsset(const std::string& name, std::unique_ptr<CADacomb::Triangulation> triangulation);
    virtual ~TriangulationAsset();

    CADacomb::Triangulation* getTriangulation() const;
    void                     setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri);

    const glm::vec3& getMeshColor() const;
    void             setMeshColor(const glm::vec3& color);

    bool isDirty() const;
    void markClean();

  private:
    std::unique_ptr<CADacomb::Triangulation> triangulation;
    glm::vec3                                meshColor = glm::vec3(0.75f, 0.78f, 0.82f);
    bool                                     dirty     = true;
};
