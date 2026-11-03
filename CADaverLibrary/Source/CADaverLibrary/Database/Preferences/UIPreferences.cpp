#include "Database/Preferences/UIPreferences.h"

UIPreferences::UIPreferences() {}

UIPreferences::~UIPreferences() {}

nlohmann::json UIPreferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["uiScale"]              = uiScale;
    jsonObject["themeIndex"]           = themeIndex;
    jsonObject["backgroundColor"]      = { backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a };
    jsonObject["systemConsoleEnabled"] = systemConsoleEnabled;
    return jsonObject;
}

void UIPreferences::fromJson(const nlohmann::json& jsonObject)
{
    uiScale    = jsonObject.at("uiScale").get<float>();
    themeIndex = jsonObject.at("themeIndex").get<int>();

    const auto& bgArray = jsonObject.at("backgroundColor");
    backgroundColor.r   = bgArray.at(0).get<float>();
    backgroundColor.g   = bgArray.at(1).get<float>();
    backgroundColor.b   = bgArray.at(2).get<float>();
    backgroundColor.a   = bgArray.at(3).get<float>();

    if (jsonObject.contains("systemConsoleEnabled"))
    {
        systemConsoleEnabled = jsonObject.at("systemConsoleEnabled").get<bool>();
    }
    else if (jsonObject.contains("showWindowsConsole"))
    {
        systemConsoleEnabled = jsonObject.at("showWindowsConsole").get<bool>();
    }
    else
    {
        systemConsoleEnabled = true;
    }
}

float UIPreferences::getUiScale() const
{
    return uiScale;
}

void UIPreferences::setUiScale(float scale)
{
    uiScale = scale;
}

int UIPreferences::getThemeIndex() const
{
    return themeIndex;
}

void UIPreferences::setThemeIndex(int index)
{
    themeIndex = index;
}

const glm::vec4& UIPreferences::getBackgroundColor() const
{
    return backgroundColor;
}

void UIPreferences::setBackgroundColor(const glm::vec4& color)
{
    backgroundColor = color;
}

bool UIPreferences::isSystemConsoleEnabled() const
{
    return systemConsoleEnabled;
}

void UIPreferences::setSystemConsoleEnabled(bool enabled)
{
    systemConsoleEnabled = enabled;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIPreferences/DefaultState")
{
    UIPreferences prefs;
    CHECK(prefs.isSystemConsoleEnabled() == true);
    CHECK(prefs.getUiScale() == 1.0f);
    CHECK(prefs.getThemeIndex() == 0);
}

TEST_CASE("UIPreferences/JsonSerialization")
{
    UIPreferences prefs;
    prefs.setSystemConsoleEnabled(false);
    prefs.setUiScale(1.5f);
    prefs.setThemeIndex(2);

    nlohmann::json json = prefs.toJson();
    CHECK(json["systemConsoleEnabled"] == false);
    CHECK(json["uiScale"] == 1.5f);
    CHECK(json["themeIndex"] == 2);

    UIPreferences loaded;
    loaded.fromJson(json);
    CHECK(loaded.isSystemConsoleEnabled() == false);
    CHECK(loaded.getUiScale() == 1.5f);
    CHECK(loaded.getThemeIndex() == 2);
}
#endif
