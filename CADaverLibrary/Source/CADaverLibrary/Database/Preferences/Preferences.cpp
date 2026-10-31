#include "Database/Preferences/Preferences.h"

Preferences::Preferences() {}

Preferences::~Preferences() {}

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
