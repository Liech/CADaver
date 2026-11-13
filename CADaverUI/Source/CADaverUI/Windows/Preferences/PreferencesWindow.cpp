#include "Windows/Preferences/PreferencesWindow.h"

#include "Database/Database.h"
#include "Database/Preferences/Preferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Preferences/UIPreferencesTab.h"

#include <fstream>
#include <imgui.h>
#include <nlohmann/json.hpp>

PreferencesWindow::PreferencesWindow()
{
    tabs = tabFactory.createTabs();
}

PreferencesWindow::~PreferencesWindow() {}

std::string PreferencesWindow::getType() const
{
    return "Preferences";
}

std::string PreferencesWindow::getDescription() const
{
    return "Window: Preferences\n"
           "Title: Preferences\n"
           "Purpose: Configure application preferences across UI theme, camera, rendering, and sparse voxel octree (SVO) settings.\n\n"
           "Parameters:\n"
           "  - Tabs: UI, Camera, Render, SVO\n\n"
           "Dispatched Command: UpdateUIPreferences, UpdateCameraPreferences, UpdateRenderPreferences, UpdateSVOPreferences (sync/async)";
}

void PreferencesWindow::selectTab(const std::string& tabName)
{
    selectedTab = tabName;
}

const std::string& PreferencesWindow::getSelectedTab() const
{
    return selectedTab;
}

PreferenceTabFactory& PreferencesWindow::getTabFactory()
{
    return tabFactory;
}

const PreferenceTabFactory& PreferencesWindow::getTabFactory() const
{
    return tabFactory;
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
        Polyglot::Node uiRes = database.execute({
          {    "type", "GetUIPreferences" },
          { "version",                  1 }
        });
        UIPreferences  uiPrefs;
        uiPrefs.fromNode(uiRes["preferences"]);
        UIPreferencesTab::apply(uiPrefs);
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
              Polyglot::Node uiCommand = {
                  {        "type",                   "UpdateUIPreferences" },
                  {     "version",                                       1 },
                  { "preferences", loadedPrefs.getUIPreferences().toNode() }
              };
              database.execute(uiCommand);

              Polyglot::Node cameraCommand = {
                  {        "type",                   "UpdateCameraPreferences" },
                  {     "version",                                           1 },
                  { "preferences", loadedPrefs.getCameraPreferences().toNode() }
              };
              database.execute(cameraCommand);

              Polyglot::Node renderCommand = {
                  {        "type",                   "UpdateRenderPreferences" },
                  {     "version",                                           1 },
                  { "preferences", loadedPrefs.getRenderPreferences().toNode() }
              };
              database.execute(renderCommand);

              Polyglot::Node svoCommand = {
                  {        "type",                   "UpdateSVOPreferences" },
                  {     "version",                                        1 },
                  { "preferences", loadedPrefs.getSVOPreferences().toNode() }
              };
              database.execute(svoCommand);
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
        tabs = tabFactory.createTabs();
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 420.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Preferences", &openState))
    {
        handleEscapeClose();
        bool modified = false;

        if (ImGui::BeginTabBar("PreferencesTabs"))
        {
            for (auto& tab : tabs)
            {
                ImGuiTabItemFlags flags = (tab->getTitle() == selectedTab) ? ImGuiTabItemFlags_SetSelected : ImGuiTabItemFlags_None;
                if (ImGui::BeginTabItem(tab->getTitle().c_str(), nullptr, flags))
                {
                    if (tab->getTitle() == selectedTab)
                    {
                        selectedTab.clear();
                    }
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
            defer->defer(
              [this]()
              {
                  Preferences prefs;
                  prefs.getUIPreferences().fromNode(gatedDatabase->execute({
                    {    "type", "GetUIPreferences" },
                    { "version",                  1 }
                  })["preferences"]);
                  prefs.getCameraPreferences().fromNode(gatedDatabase->execute({
                    {    "type", "GetCameraPreferences" },
                    { "version",                      1 }
                  })["preferences"]);
                  prefs.getRenderPreferences().fromNode(gatedDatabase->execute({
                    {    "type", "GetRenderPreferences" },
                    { "version",                      1 }
                  })["preferences"]);
                  prefs.getSVOPreferences().fromNode(gatedDatabase->execute({
                    {    "type", "GetSVOPreferences" },
                    { "version",                   1 }
                  })["preferences"]);
                  save(prefs);
              });
        }
    }
    ImGui::End();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PreferencesWindow/TypeAndDescription")
{
    PreferencesWindow window;
    CHECK(window.getType() == "Preferences");
    CHECK(!window.getDescription().empty());
}

TEST_CASE("PreferencesWindow/TabSelection")
{
    PreferencesWindow window;
    CHECK(window.getSelectedTab().empty());
    window.selectTab("Render");
    CHECK(window.getSelectedTab() == "Render");
}
#endif
