#include "Database/Asset.h"

Asset::Asset(const std::string& nameInput, AssetType typeInput)
  : guid(CADacomb::GUID::random())
  , name(nameInput)
  , type(typeInput)
{
}

Asset::Asset(const CADacomb::GUID& guidInput, const std::string& nameInput, AssetType typeInput)
  : guid(guidInput)
  , name(nameInput)
  , type(typeInput)
{
}

Asset::~Asset() {}

const CADacomb::GUID& Asset::getGuid() const
{
    return guid;
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
        case AssetType::ProgressBar:
            return "Progress";
    }
    return "Unknown";
}
