#include "UI/Menu/CreateMenu.h"

#include "Windows/WindowContainer.h"

#include <imgui.h>

CreateMenu::CreateMenu() {}

CreateMenu::~CreateMenu() {}

void CreateMenu::render(WindowContainer& windowContainer)
{
    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Landscape..."))
        {
            windowContainer.open("LandscapeGeneratorWindow");
        }

        ImGui::EndMenu();
    }
}
