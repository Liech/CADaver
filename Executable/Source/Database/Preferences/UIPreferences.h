#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>

// Headless data model managing UI theme, scale, and background color preferences.
class UIPreferences
{
  public:
    UIPreferences();
    virtual ~UIPreferences();

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    float            getUiScale() const;
    void             setUiScale(float scale);
    int              getThemeIndex() const;
    void             setThemeIndex(int index);
    const glm::vec4& getBackgroundColor() const;
    void             setBackgroundColor(const glm::vec4& color);

  private:
    glm::vec4 backgroundColor = glm::vec4(0.12f, 0.12f, 0.14f, 1.0f);
    float     uiScale         = 1.0f;
    int       themeIndex      = 0;
};
