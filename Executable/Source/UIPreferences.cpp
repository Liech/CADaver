#include "UIPreferences.h"

#include "PreferencesData.h"

#include <imgui.h>

UIPreferences::UIPreferences() {}

UIPreferences::~UIPreferences() {}

const std::string& UIPreferences::getTitle() const
{
    return title;
}

bool UIPreferences::render(PreferencesData& preferences)
{
    bool modified = false;

    ImGui::Text("Interface Appearance");
    ImGui::Separator();
    ImGui::Spacing();

    float currentScale = preferences.getUiScale();
    if (ImGui::SliderFloat("UI Scale", &currentScale, 0.5f, 2.5f, "%.2f"))
    {
        preferences.setUiScale(currentScale);
        preferences.apply();
        preferences.save();
        modified = true;
    }

    const char* themes[]     = { "Dark", "Light", "Classic" };
    int         currentTheme = preferences.getThemeIndex();
    if (ImGui::Combo("Color Theme", &currentTheme, themes, 3))
    {
        preferences.setThemeIndex(currentTheme);
        preferences.apply();
        preferences.save();
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Reset to Defaults"))
    {
        preferences.setUiScale(1.0f);
        preferences.setThemeIndex(0);
        preferences.apply();
        preferences.save();
        modified = true;
    }

    return modified;
}
