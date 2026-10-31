#include "Windows/Preferences/PreferencesWindow.h"

#include "Database/Database.h"
#include "Database/Preferences/Preferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Preferences/PreferenceTabFactory.h"
#include "Windows/Preferences/UIPreferencesTab.h"

#include <fstream>
#include <imgui.h>
#include <nlohmann/json.hpp>

PreferencesWindow::PreferencesWindow()
{
    tabs = PreferenceTabFactory::getInstance().createTabs();
}

PreferencesWindow::~PreferencesWindow() {}

std::string PreferencesWindow::getType() const
{
    return "PreferencesWindow";
}

std::string PreferencesWindow::getDescription() const
{
    return "Preferences";
}

void PreferencesWindow::load(const std::string& filepath)
{
    if (gatedDatabase && defer)
    {
        load(*gatedDatabase, *defer, filepath);
    }
}

void PreferencesWindow::load(GatedDatabase& database, Defer& deferTarget, const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        return;
    }

    try
    {
        nlohmann::json jsonObject;
        file >> jsonObject;

        Preferences loadedPrefs;
        loadedPrefs.fromJson(jsonObject);

        UIPreferencesTab::apply(loadedPrefs.getUIPreferences());

        deferTarget.defer(
          [&database, loadedPrefs]()
          {
              database.execute({
                {        "type",                   "UpdateUIPreferences" },
                {     "version",                                       1 },
                { "preferences", loadedPrefs.getUIPreferences().toJson() }
              });
              database.execute({
                {        "type",                   "UpdateCameraPreferences" },
                {     "version",                                           1 },
                { "preferences", loadedPrefs.getCameraPreferences().toJson() }
              });
              database.execute({
                {        "type",                   "UpdateRenderPreferences" },
                {     "version",                                           1 },
                { "preferences", loadedPrefs.getRenderPreferences().toJson() }
              });
              database.execute({
                {        "type",                   "UpdateSVOPreferences" },
                {     "version",                                        1 },
                { "preferences", loadedPrefs.getSVOPreferences().toJson() }
              });
          });
    }
    catch (const nlohmann::json::exception&)
    {
    }
}

void PreferencesWindow::save(const Preferences& preferences, const std::string& filepath) const
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        return;
    }

    file << preferences.toJson().dump(4);
}

void PreferencesWindow::render()
{
    if (!openState || !gatedDatabase || !defer)
    {
        return;
    }

    if (tabs.empty())
    {
        tabs = PreferenceTabFactory::getInstance().createTabs();
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 420.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Preferences", &openState))
    {
        bool modified = false;

        if (ImGui::BeginTabBar("PreferencesTabs"))
        {
            for (auto& tab : tabs)
            {
                if (ImGui::BeginTabItem(tab->getTitle().c_str()))
                {
                    if (tab->render(*gatedDatabase, *defer))
                    {
                        modified = true;
                    }
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        if (modified)
        {
            save(gatedDatabase->get().getPreferences());
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PreferencesWindow/TypeAndDescription")
{
    PreferencesWindow window;
    CHECK(window.getType() == "PreferencesWindow");
    CHECK(window.getDescription() == "Preferences");
}
#endif
