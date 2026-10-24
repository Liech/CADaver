#include "UIPreferences.h"

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
    if (jsonObject.contains("uiScale") && jsonObject["uiScale"].is_number())
    {
        uiScale = jsonObject["uiScale"].get<float>();
    }

    if (jsonObject.contains("themeIndex") && jsonObject["themeIndex"].is_number_integer())
    {
        themeIndex = jsonObject["themeIndex"].get<int>();
    }

    if (jsonObject.contains("backgroundColor") && jsonObject["backgroundColor"].is_array() && jsonObject["backgroundColor"].size() >= 4)
    {
        backgroundColor.r = jsonObject["backgroundColor"][0].get<float>();
        backgroundColor.g = jsonObject["backgroundColor"][1].get<float>();
        backgroundColor.b = jsonObject["backgroundColor"][2].get<float>();
        backgroundColor.a = jsonObject["backgroundColor"][3].get<float>();
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
