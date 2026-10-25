#include "Database/Preferences/RenderPreferences.h"

RenderPreferences::RenderPreferences() {}

RenderPreferences::~RenderPreferences() {}

nlohmann::json RenderPreferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["meshColor"]      = { meshColor.r, meshColor.g, meshColor.b };
    jsonObject["wireframeMode"]  = wireframeMode;
    jsonObject["wireframeColor"] = { wireframeColor.r, wireframeColor.g, wireframeColor.b };
    jsonObject["cullMode"]       = cullMode;
    return jsonObject;
}

void RenderPreferences::fromJson(const nlohmann::json& jsonObject)
{
    const auto& meshColorArray = jsonObject.at("meshColor");
    meshColor.r                = meshColorArray.at(0).get<float>();
    meshColor.g                = meshColorArray.at(1).get<float>();
    meshColor.b                = meshColorArray.at(2).get<float>();

    wireframeMode = jsonObject.at("wireframeMode").get<int>();

    const auto& wireColorArray = jsonObject.at("wireframeColor");
    wireframeColor.r           = wireColorArray.at(0).get<float>();
    wireframeColor.g           = wireColorArray.at(1).get<float>();
    wireframeColor.b           = wireColorArray.at(2).get<float>();

    cullMode = jsonObject.at("cullMode").get<int>();
}

const glm::vec3& RenderPreferences::getMeshColor() const
{
    return meshColor;
}

void RenderPreferences::setMeshColor(const glm::vec3& color)
{
    meshColor = color;
}

int RenderPreferences::getWireframeMode() const
{
    return wireframeMode;
}

void RenderPreferences::setWireframeMode(int mode)
{
    wireframeMode = mode;
}

const glm::vec3& RenderPreferences::getWireframeColor() const
{
    return wireframeColor;
}

void RenderPreferences::setWireframeColor(const glm::vec3& color)
{
    wireframeColor = color;
}

int RenderPreferences::getCullMode() const
{
    return cullMode;
}

void RenderPreferences::setCullMode(int mode)
{
    cullMode = mode;
}
