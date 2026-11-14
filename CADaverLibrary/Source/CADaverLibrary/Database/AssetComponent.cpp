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
    markModified();
}

size_t AssetComponent::getRevision() const
{
    return revision;
}

void AssetComponent::markModified()
{
    revision++;
}

nlohmann::json AssetComponent::toJson() const
{
    return {
        {       "Version",               1 },
        { "componentGuid", guid.toString() },
        {          "type",       getType() },
        {          "name",            name },
        {          "data",    toDataJson() }
    };
}

void AssetComponent::fromJson(const nlohmann::json& jsonObject)
{
    jsonObject.at("Version").get<int>();
    guid = CADacomb::GUID(jsonObject.at("componentGuid").get<std::string>());
    name = jsonObject.at("name").get<std::string>();
    fromDataJson(jsonObject.at("data"));
}
