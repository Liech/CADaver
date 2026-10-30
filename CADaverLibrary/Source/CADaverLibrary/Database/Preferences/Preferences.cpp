#include "Database/Preferences/Preferences.h"

#include <fstream>

Preferences::Preferences() {}

Preferences::~Preferences() {}

void Preferences::load(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        return;
    }

    try
    {
        nlohmann::json jsonObject;
        file >> jsonObject;
        fromJson(jsonObject);
    }
    catch (...)
    {
    }
}

void Preferences::save(const std::string& filepath) const
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        return;
    }

    file << toJson().dump(4);
}

nlohmann::json Preferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["ui"]     = uiPreferences.toJson();
    jsonObject["camera"] = cameraPreferences.toJson();
    jsonObject["render"] = renderPreferences.toJson();
    jsonObject["svo"]    = svoPreferences.toJson();
    return jsonObject;
}

void Preferences::fromJson(const nlohmann::json& jsonObject)
{
    uiPreferences.fromJson(jsonObject.at("ui"));
    cameraPreferences.fromJson(jsonObject.at("camera"));
    renderPreferences.fromJson(jsonObject.at("render"));
    svoPreferences.fromJson(jsonObject.at("svo"));
}

UIPreferences& Preferences::getUIPreferences()
{
    return uiPreferences;
}

const UIPreferences& Preferences::getUIPreferences() const
{
    return uiPreferences;
}

CameraPreferences& Preferences::getCameraPreferences()
{
    return cameraPreferences;
}

const CameraPreferences& Preferences::getCameraPreferences() const
{
    return cameraPreferences;
}

RenderPreferences& Preferences::getRenderPreferences()
{
    return renderPreferences;
}

const RenderPreferences& Preferences::getRenderPreferences() const
{
    return renderPreferences;
}

SVOPreferences& Preferences::getSVOPreferences()
{
    return svoPreferences;
}

const SVOPreferences& Preferences::getSVOPreferences() const
{
    return svoPreferences;
}
