#include "UI/Menu/FileMenu.h"

#include "UI/Preferences/PreferencesWindow.h"
#include "Windows/WindowContainer.h"

#include <imgui.h>

FileMenu::FileMenu() {}

FileMenu::~FileMenu() {}

void FileMenu::render(WindowContainer& windowContainer, PreferencesWindow& preferencesWindow, bool& quitRequested)
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Load"))
        {
            if (ImGui::MenuItem("STL..."))
            {
                windowContainer.open("LoadStlAssetWindow");
            }
            if (ImGui::MenuItem("STEP..."))
            {
                windowContainer.open("LoadStepAssetWindow");
            }
            if (ImGui::MenuItem("SVO..."))
            {
                windowContainer.open("LoadSvoAssetWindow");
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Preferences..."))
        {
            preferencesWindow.setOpen(true);
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Quit", "Alt+F4"))
        {
            quitRequested = true;
        }

        ImGui::EndMenu();
    }
}
