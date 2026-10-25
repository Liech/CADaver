#include "PreferencesWindow.h"

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
                    modified = true;
                    GatedDatabase::getInstance().execute({
                      {        "type",                          "UpdateUIPreferences" },
                      {     "version",                                              1 },
                      { "preferences", workingPreferences.getUIPreferences().toJson() }
                    });
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera"))
            {
                if (cameraPreferencesView.render(workingPreferences.getCameraPreferences(), camera))
                {
                    modified = true;
                    GatedDatabase::getInstance().execute({
                      {        "type",                          "UpdateCameraPreferences" },
                      {     "version",                                                  1 },
                      { "preferences", workingPreferences.getCameraPreferences().toJson() }
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
                    modified = true;
                    GatedDatabase::getInstance().execute({
                      {        "type",                          "UpdateRenderPreferences" },
                      {     "version",                                                  1 },
                      { "preferences", workingPreferences.getRenderPreferences().toJson() }
                    });
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("SVO"))
            {
                if (svoPreferencesView.render(workingPreferences.getSVOPreferences()))
                {
                    modified = true;
                    GatedDatabase::getInstance().execute({
                      {        "type",                          "UpdateSVOPreferences" },
                      {     "version",                                               1 },
                      { "preferences", workingPreferences.getSVOPreferences().toJson() }
                    });
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
