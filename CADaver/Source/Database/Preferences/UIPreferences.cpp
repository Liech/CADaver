#include "Database/Preferences/UIPreferences.h"

UIPreferences::UIPreferences() {}

UIPreferences::~UIPreferences() {}

nlohmann::json UIPreferences::toJson() const
{
    nlohmann::json jsonObject;
    jsonObject["uiScale"]         = uiScale;
    jsonObject["themeIndex"]      = themeIndex;
    jsonObject["backgroundColor"] = { backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a };
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
