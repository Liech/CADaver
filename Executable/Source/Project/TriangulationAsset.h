#pragma once

#include "Asset.h"
#include "CADacomb/Triangle/Triangulation.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

namespace Vulcant
{
    class VulcantDevice;
    class VulcantGraphicCommand;
    class VulcantImage;
}

namespace Vulcant::Toolkit
{
    class MeshRenderer;
    struct MeshVertex;
    struct SceneData;
}

class TriangulationAsset : public Asset
{
  public:
    TriangulationAsset(const std::string& name);
    TriangulationAsset(const std::string& name, std::unique_ptr<CADacomb::Triangulation> triangulation);
    virtual ~TriangulationAsset();

    virtual void                   render(std::vector<Vulcant::Toolkit::MeshVertex>& vertices, bool isSelected = false) override;
    virtual Vulcant::VulcantImage* render(Vulcant::VulcantDevice& device, const glm::ivec2& resolution, const Vulcant::Toolkit::SceneData& sceneData) override;

    CADacomb::Triangulation* getTriangulation() const;
    void                     setTriangulation(std::unique_ptr<CADacomb::Triangulation> tri);

    const glm::vec3& getMeshColor() const;
    void             setMeshColor(const glm::vec3& color);

  private:
    std::unique_ptr<CADacomb::Triangulation>        triangulation;
    std::unique_ptr<Vulcant::Toolkit::MeshRenderer> meshRenderer;
    std::unique_ptr<Vulcant::VulcantGraphicCommand> drawCmd;
    glm::ivec2                                      currentResolution = glm::ivec2(0, 0);
    glm::vec3                                       meshColor         = glm::vec3(0.75f, 0.78f, 0.82f);
    glm::vec4                                       backgroundColor   = glm::vec4(0.12f, 0.12f, 0.14f, 1.0f);
    bool                                            meshDirty         = true;
};
