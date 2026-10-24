#include "PreferencesWindow.h"

#include "Preferences/Preferences.h"

#include <imgui.h>

PreferencesWindow::PreferencesWindow() {}

PreferencesWindow::~PreferencesWindow() {}

void PreferencesWindow::render(Preferences& preferences, Vulcant::Toolkit::Camera* camera)
{
    if (!openState)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(480.0f, 420.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Preferences", &openState))
    {
        bool modified = false;

        if (ImGui::BeginTabBar("PreferencesTabs"))
        {
            if (ImGui::BeginTabItem("UI"))
            {
                if (uiPreferencesView.render(preferences.getUIPreferences()))
                {
                    modified = true;
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera"))
            {
                if (cameraPreferencesView.render(preferences.getCameraPreferences(), camera))
                {
                    modified = true;
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Render"))
            {
                if (renderPreferencesView.render(preferences.getRenderPreferences()))
                {
                    modified = true;
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        if (modified)
        {
            preferences.save();
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
