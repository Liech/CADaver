#include "PreferencesWindow.h"

#include "Database/Preferences/Preferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"

#include <fstream>
#include <imgui.h>
#include <nlohmann/json.hpp>

PreferencesWindow::PreferencesWindow() {}

PreferencesWindow::~PreferencesWindow() {}

void PreferencesWindow::load(GatedDatabase& gatedDatabase, const std::string& filepath)
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

        uiPreferencesView.apply(loadedPrefs.getUIPreferences());

        Defer::getInstance().defer(
          [&gatedDatabase, loadedPrefs]()
          {
              gatedDatabase.execute({
                {        "type",                   "UpdateUIPreferences" },
                {     "version",                                       1 },
                { "preferences", loadedPrefs.getUIPreferences().toJson() }
              });
              gatedDatabase.execute({
                {        "type",                   "UpdateCameraPreferences" },
                {     "version",                                           1 },
                { "preferences", loadedPrefs.getCameraPreferences().toJson() }
              });
              gatedDatabase.execute({
                {        "type",                   "UpdateRenderPreferences" },
                {     "version",                                           1 },
                { "preferences", loadedPrefs.getRenderPreferences().toJson() }
              });
              gatedDatabase.execute({
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

void PreferencesWindow::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Vulcant::Toolkit::Camera* camera)
{
    if (!openState)
    {
        return;
    }

    Preferences workingPreferences = gatedDatabase.get().getPreferences();

    ImGui::SetNextWindowSize(ImVec2(480.0f, 420.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Preferences", &openState))
    {
        bool modified = false;

        if (ImGui::BeginTabBar("PreferencesTabs"))
        {
            if (ImGui::BeginTabItem("UI"))
            {
                if (uiPreferencesView.render(workingPreferences.getUIPreferences()))
                {
                    modified  = true;
                    auto json = workingPreferences.getUIPreferences().toJson();
                    Defer::getInstance().defer(
                      [&gatedDatabase, json]()
                      {
                          gatedDatabase.execute({
                            {        "type", "UpdateUIPreferences" },
                            {     "version",                     1 },
                            { "preferences",                  json }
                          });
                      });
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera"))
            {
                if (cameraPreferencesView.render(workingPreferences.getCameraPreferences(), camera))
                {
                    modified  = true;
                    auto json = workingPreferences.getCameraPreferences().toJson();
                    Defer::getInstance().defer(
                      [&gatedDatabase, json]()
                      {
                          gatedDatabase.execute({
                            {        "type", "UpdateCameraPreferences" },
                            {     "version",                         1 },
                            { "preferences",                      json }
                          });
                      });
                }
                if (workingPreferences.getCameraPreferences().isResetRequested())
                {
                    resetRequested = true;
                    workingPreferences.getCameraPreferences().clearResetRequested();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Render"))
            {
                if (renderPreferencesView.render(workingPreferences.getRenderPreferences()))
                {
                    modified  = true;
                    auto json = workingPreferences.getRenderPreferences().toJson();
                    Defer::getInstance().defer(
                      [&gatedDatabase, json]()
                      {
                          gatedDatabase.execute({
                            {        "type", "UpdateRenderPreferences" },
                            {     "version",                         1 },
                            { "preferences",                      json }
                          });
                      });
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("SVO"))
            {
                if (svoPreferencesView.render(workingPreferences.getSVOPreferences()))
                {
                    modified  = true;
                    auto json = workingPreferences.getSVOPreferences().toJson();
                    Defer::getInstance().defer(
                      [&gatedDatabase, json]()
                      {
                          gatedDatabase.execute({
                            {        "type", "UpdateSVOPreferences" },
                            {     "version",                      1 },
                            { "preferences",                   json }
                          });
                      });
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Undo"))
            {
                if (undoPreferencesView.render(undoStack))
                {
                    modified = true;
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        if (modified)
        {
            save(workingPreferences);
        }
    }
    ImGui::End();
}

bool PreferencesWindow::isOpen() const
{
    return openState;
}

void PreferencesWindow::setOpen(bool open)
{
    openState = open;
}

bool PreferencesWindow::isResetRequested() const
{
    return resetRequested;
}

void PreferencesWindow::clearResetRequested()
{
    resetRequested = false;
}

UIPreferencesView& PreferencesWindow::getUIPreferencesView()
{
    return uiPreferencesView;
}

CameraPreferencesView& PreferencesWindow::getCameraPreferencesView()
{
    return cameraPreferencesView;
}

RenderPreferencesView& PreferencesWindow::getRenderPreferencesView()
{
    return renderPreferencesView;
}

SVOPreferencesView& PreferencesWindow::getSVOPreferencesView()
{
    return svoPreferencesView;
}

UndoPreferencesView& PreferencesWindow::getUndoPreferencesView()
{
    return undoPreferencesView;
}
