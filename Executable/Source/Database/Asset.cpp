#include "Database/Asset.h"

Asset::Asset(const std::string& nameInput, AssetType typeInput)
  : name(nameInput)
  , type(typeInput)
{
}

Asset::~Asset() {}

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
