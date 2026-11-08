#include "Database/AssetComponent.h"

AssetComponent::AssetComponent()
  : guid(CADacomb::GUID::random())
{
}

AssetComponent::~AssetComponent() {}

const CADacomb::GUID& AssetComponent::getGuid() const
{
    return guid;
}

const std::string& AssetComponent::getName() const
{
    return name;
}

void AssetComponent::setName(const std::string& newName)
{
    name = newName;
}
