#pragma once

#include "CADacomb/Triangle/Triangulation.h"
#include "Database/Asset.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace Library
{
    class CADShape;
}

// Headless data asset representing CAD model geometry and its tessellated mesh.
class CadAsset : public Asset
{
  public:
    CadAsset(const std::string& name);
    CadAsset(const std::string& name, const std::string& filePath);
    CadAsset(const std::string& name, std::shared_ptr<Library::CADShape> cadShape);
    virtual ~CadAsset();

    bool loadFromFile(const std::string& filePath);

    const std::string& getFilePath() const;
    void               setFilePath(const std::string& path);

    const Library::CADShape* getCadShape() const;
    Library::CADShape*       getCadShape();
    void                     setCadShape(std::shared_ptr<Library::CADShape> shape);

    const CADacomb::Triangulation* getTriangulation() const;
    CADacomb::Triangulation*       getTriangulation();
    void                           setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri);

    const glm::vec3& getMeshColor() const;
    void             setMeshColor(const glm::vec3& color);

    size_t getSizeInBytes() const override;

  private:
    void triangulateShape();

    std::string                              filePath;
    std::shared_ptr<Library::CADShape>       cadShape;
    std::unique_ptr<CADacomb::Triangulation> triangulation;
    glm::vec3                                meshColor = glm::vec3(0.68f, 0.78f, 0.90f);
};
