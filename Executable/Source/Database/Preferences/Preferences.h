#pragma once

#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Database/Preferences/SVOPreferences.h"
#include "Database/Preferences/UIPreferences.h"

#include <nlohmann/json.hpp>
#include <string>

// Headless root container for application preferences, managing serialization to/from JSON.
class Preferences
{
  public:
    Preferences();
    virtual ~Preferences();

    void load(const std::string& filepath = "preferences.json");
    void save(const std::string& filepath = "preferences.json") const;

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    UIPreferences&       getUIPreferences();
    const UIPreferences& getUIPreferences() const;

    CameraPreferences&       getCameraPreferences();
    const CameraPreferences& getCameraPreferences() const;

    RenderPreferences&       getRenderPreferences();
    const RenderPreferences& getRenderPreferences() const;

    SVOPreferences&       getSVOPreferences();
    const SVOPreferences& getSVOPreferences() const;

  private:
    UIPreferences     uiPreferences;
    CameraPreferences cameraPreferences;
    RenderPreferences renderPreferences;
    SVOPreferences    svoPreferences;
};
