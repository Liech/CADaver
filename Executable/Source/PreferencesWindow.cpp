#include "PreferencesWindow.h"

#include "PreferencesData.h"
#include "UIPreferences.h"

#include <imgui.h>

PreferencesWindow::PreferencesWindow()
{
    uiPreferences = std::make_unique<UIPreferences>();
}

PreferencesWindow::~PreferencesWindow() {}

void PreferencesWindow::render(PreferencesData& preferences)
{
    if (!isOpenState)
    {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(500.0f, 340.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Preferences", &isOpenState, ImGuiWindowFlags_NoCollapse))
    {
        if (ImGui::BeginTabBar("PreferencesTabs"))
        {
            if (ImGui::BeginTabItem(uiPreferences->getTitle().c_str()))
            {
                uiPreferences->render(preferences);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

bool PreferencesWindow::isOpen() const
{
    return isOpenState;
}

void PreferencesWindow::setOpen(bool open)
{
    isOpenState = open;
}
