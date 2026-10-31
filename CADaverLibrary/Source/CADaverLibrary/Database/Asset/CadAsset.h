#pragma once

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Asset.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace CADacomb::CAD
{
    class Shape;
}

// Headless data asset representing CAD model geometry and its tessellated mesh.
class CadAsset : public Asset
{
  public:
    CadAsset();
    virtual ~CadAsset();

    std::string getType() const override;

    const CADacomb::CAD::Shape* getCadShape() const;
    CADacomb::CAD::Shape*       getCadShape();
    void                        setCadShape(std::shared_ptr<CADacomb::CAD::Shape> shape);

    const CADacomb::Triangulation* getTriangulation() const;
    CADacomb::Triangulation*       getTriangulation();
    void                           setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri);

    const glm::vec3& getMeshColor() const;
    void             setMeshColor(const glm::vec3& color);

    size_t getSizeInBytes() const override;

  private:
    void triangulateShape();

    std::shared_ptr<CADacomb::CAD::Shape>    cadShape;
    std::unique_ptr<CADacomb::Triangulation> triangulation;
    glm::vec3                                meshColor = glm::vec3(0.68f, 0.78f, 0.90f);
};
