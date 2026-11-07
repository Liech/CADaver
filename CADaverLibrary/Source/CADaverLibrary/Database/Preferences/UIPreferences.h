#pragma once

#include "Polyglot/Node.h"

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>

// Headless data model managing UI theme, scale, background color, and system console visibility preferences.
class UIPreferences
{
  public:
    UIPreferences();
    virtual ~UIPreferences();

    Polyglot::Node toNode() const;
    void           fromNode(const Polyglot::Node& node);

    nlohmann::json toJson() const;
    void           fromJson(const nlohmann::json& jsonObject);

    float            getUiScale() const;
    void             setUiScale(float scale);
    int              getThemeIndex() const;
    void             setThemeIndex(int index);
    const glm::vec4& getBackgroundColor() const;
    void             setBackgroundColor(const glm::vec4& color);
    bool             isSystemConsoleEnabled() const;
    void             setSystemConsoleEnabled(bool enabled);

  private:
    glm::vec4 backgroundColor      = glm::vec4(0.12f, 0.12f, 0.14f, 1.0f);
    float     uiScale              = 1.0f;
    int       themeIndex           = 0;
    bool      systemConsoleEnabled = true;
};
