#include "PreferencesWindow.h"

#include "Commands/Defer.h"
#include "Commands/GatedDatabase.h"
#include "Database/Preferences/Preferences.h"

#include <imgui.h>

PreferencesWindow::PreferencesWindow() {}

PreferencesWindow::~PreferencesWindow() {}

void PreferencesWindow::render(const Preferences& preferences, Vulcant::Toolkit::Camera* camera)
{
    if (!openState)
    {
        return;
    }

    Preferences workingPreferences = preferences;

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
                      [json]()
                      {
                          GatedDatabase::getInstance().execute(
                            {
                              {        "type", "UpdateUIPreferences" },
                              {     "version",                     1 },
                              { "preferences",                  json }
                          },
                            true);
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
                      [json]()
                      {
                          GatedDatabase::getInstance().execute(
                            {
                              {        "type", "UpdateCameraPreferences" },
                              {     "version",                         1 },
                              { "preferences",                      json }
                          },
                            true);
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
                      [json]()
                      {
                          GatedDatabase::getInstance().execute(
                            {
                              {        "type", "UpdateRenderPreferences" },
                              {     "version",                         1 },
                              { "preferences",                      json }
                          },
                            true);
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
                      [json]()
                      {
                          GatedDatabase::getInstance().execute(
                            {
                              {        "type", "UpdateSVOPreferences" },
                              {     "version",                      1 },
                              { "preferences",                   json }
                          },
                            true);
                      });
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Undo"))
            {
                const auto& undoStack = GatedDatabase::getInstance().get().getUndoStack();
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
            workingPreferences.save();
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
