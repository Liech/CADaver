#include "UI/Menu/FileMenu.h"

#include "Windows/Windows.h"

#include <imgui.h>

FileMenu::FileMenu() {}

FileMenu::~FileMenu() {}

void FileMenu::render(Windows& windows, bool& quitRequested)
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::BeginMenu("Load"))
        {
            if (ImGui::MenuItem("STL..."))
            {
                windows.open("LoadStlAssetWindow");
            }
            if (ImGui::MenuItem("STEP..."))
            {
                windows.open("LoadStepAssetWindow");
            }
            if (ImGui::MenuItem("SVO..."))
            {
                windows.open("LoadSvoAssetWindow");
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Preferences..."))
        {
            windows.open("PreferencesWindow");
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Quit", "Alt+F4"))
        {
            quitRequested = true;
        }

        ImGui::EndMenu();
    }
}
