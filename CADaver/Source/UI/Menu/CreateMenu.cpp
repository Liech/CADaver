#include "UI/Menu/CreateMenu.h"

#include "Windows/Windows.h"

#include <imgui.h>

CreateMenu::CreateMenu() {}

CreateMenu::~CreateMenu() {}

void CreateMenu::render(Windows& windows)
{
    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Landscape..."))
        {
            windows.open("LandscapeGenerator");
        }

        ImGui::EndMenu();
    }
}
