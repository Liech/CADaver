#pragma once

#include "AssetType.h"

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Vulcant
{
    class VulcantDevice;
    class VulcantImage;
}

namespace Vulcant::Toolkit
{
    struct MeshVertex;
    struct SceneData;
}

class Asset
{
  public:
    Asset(const std::string& name, AssetType type);
    virtual ~Asset();

    virtual void                   render(std::vector<Vulcant::Toolkit::MeshVertex>& vertices, bool isSelected = false);
    virtual Vulcant::VulcantImage* render(Vulcant::VulcantDevice& device, const glm::ivec2& resolution, const Vulcant::Toolkit::SceneData& sceneData);

    const std::string& getName() const;
    void               setName(const std::string& name);

    AssetType   getType() const;
    std::string getTypeName() const;

  private:
    std::string name;
    AssetType   type;
};
