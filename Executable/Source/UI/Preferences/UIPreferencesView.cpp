#include "UIPreferencesView.h"

#include "Preferences/UIPreferences.h"

#include <imgui.h>

UIPreferencesView::UIPreferencesView() {}

UIPreferencesView::~UIPreferencesView() {}

void UIPreferencesView::apply(const UIPreferences& preferences)
{
    ImGuiIO& io        = ImGui::GetIO();
    io.FontGlobalScale = preferences.getUiScale();

    switch (preferences.getThemeIndex())
    {
        case 1:
            ImGui::StyleColorsLight();
            break;
        case 2:
            ImGui::StyleColorsClassic();
            break;
        default:
            ImGui::StyleColorsDark();
            break;
    }
}

bool UIPreferencesView::render(UIPreferences& preferences)
{
    bool modified = false;

    ImGui::Text("Interface Appearance");
    ImGui::Separator();
    ImGui::Spacing();

    float currentScale = preferences.getUiScale();
    if (ImGui::SliderFloat("UI Scale", &currentScale, 0.5f, 2.5f, "%.2f"))
    {
        preferences.setUiScale(currentScale);
        apply(preferences);
        modified = true;
    }

    const char* themes[]     = { "Dark", "Light", "Classic" };
    int         currentTheme = preferences.getThemeIndex();
    if (ImGui::Combo("Color Theme", &currentTheme, themes, 3))
    {
        preferences.setThemeIndex(currentTheme);
        apply(preferences);
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Viewport");
    ImGui::Separator();
    ImGui::Spacing();

    glm::vec4 bgColor = preferences.getBackgroundColor();
    if (ImGui::ColorEdit4("Background Color", &bgColor[0]))
    {
        preferences.setBackgroundColor(bgColor);
        modified = true;
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Reset to Defaults"))
    {
        preferences.setUiScale(1.0f);
        preferences.setThemeIndex(0);
        preferences.setBackgroundColor(glm::vec4(0.12f, 0.12f, 0.14f, 1.0f));
        apply(preferences);
        modified = true;
    }

    return modified;
}
