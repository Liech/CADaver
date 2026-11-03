#include "Windows/Preferences/UIPreferencesTab.h"

#include "Database/Database.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Console/SystemConsole.h"

#include <imgui.h>

UIPreferencesTab::UIPreferencesTab() {}

UIPreferencesTab::~UIPreferencesTab() {}

std::string UIPreferencesTab::getTitle() const
{
    return "UI";
}

std::string UIPreferencesTab::getDescription() const
{
    return "Preference Tab: UI\n"
           "Purpose: Configure user interface appearance, including color themes (Dark, Light, Classic), UI font scale, viewport background color, and system console visibility.";
}

void UIPreferencesTab::apply(const UIPreferences& preferences)
{
    SystemConsole::setVisible(preferences.isSystemConsoleEnabled());

    if (!ImGui::GetCurrentContext())
    {
        return;
    }

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

bool UIPreferencesTab::render(GatedDatabase& database, Defer& defer)
{
    UIPreferences preferences = database.get().getPreferences().getUIPreferences();
    bool          modified    = false;

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

    bool showConsole = preferences.isSystemConsoleEnabled();
    if (ImGui::Checkbox("System Console", &showConsole))
    {
        preferences.setSystemConsoleEnabled(showConsole);
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
        preferences.setSystemConsoleEnabled(true);
        apply(preferences);
        modified = true;
    }

    if (modified)
    {
        auto json = preferences.toJson();
        defer.defer(
          [&database, json]()
          {
              database.execute({
                {        "type", "UpdateUIPreferences" },
                {     "version",                     1 },
                { "preferences",                  json }
              });
          });
    }

    return modified;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIPreferencesTab/GetTitle")
{
    UIPreferencesTab tab;
    CHECK(tab.getTitle() == "UI");
}
#endif
