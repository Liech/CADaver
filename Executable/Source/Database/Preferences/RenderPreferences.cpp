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
    if (jsonObject.contains("meshColor") && jsonObject["meshColor"].is_array() && jsonObject["meshColor"].size() >= 3)
    {
        meshColor.r = jsonObject["meshColor"][0].get<float>();
        meshColor.g = jsonObject["meshColor"][1].get<float>();
        meshColor.b = jsonObject["meshColor"][2].get<float>();
    }

    if (jsonObject.contains("wireframeMode") && jsonObject["wireframeMode"].is_number_integer())
    {
        wireframeMode = jsonObject["wireframeMode"].get<int>();
    }

    if (jsonObject.contains("wireframeColor") && jsonObject["wireframeColor"].is_array() && jsonObject["wireframeColor"].size() >= 3)
    {
        wireframeColor.r = jsonObject["wireframeColor"][0].get<float>();
        wireframeColor.g = jsonObject["wireframeColor"][1].get<float>();
        wireframeColor.b = jsonObject["wireframeColor"][2].get<float>();
    }

    if (jsonObject.contains("cullMode") && jsonObject["cullMode"].is_number_integer())
    {
        cullMode = jsonObject["cullMode"].get<int>();
    }
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
