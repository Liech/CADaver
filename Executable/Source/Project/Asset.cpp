#include "Asset.h"

#include "Vulcant/Toolkit/MeshRenderer.h"

Asset::Asset(const std::string& nameInput, AssetType typeInput)
  : name(nameInput)
  , type(typeInput)
{
}

Asset::~Asset() {}

void Asset::render(std::vector<Vulcant::Toolkit::MeshVertex>&, bool) {}

Vulcant::VulcantImage* Asset::render(Vulcant::VulcantDevice&, const glm::ivec2&, const Vulcant::Toolkit::SceneData&)
{
    return nullptr;
}

const std::string& Asset::getName() const
{
    return name;
}

void Asset::setName(const std::string& newName)
{
    name = newName;
}

AssetType Asset::getType() const
{
    return type;
}

std::string Asset::getTypeName() const
{
    switch (type)
    {
        case AssetType::Triangulation:
            return "Triangulation";
        case AssetType::Voxel:
            return "SVO (Voxel)";
        case AssetType::Cad:
            return "CAD::Shape (CAD)";
    }
    return "Unknown";
}
