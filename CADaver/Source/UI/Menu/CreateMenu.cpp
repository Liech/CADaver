#include "UI/Menu/CreateMenu.h"

#include "Windows/Generator/LandscapeGeneratorWindow.h"

#include <imgui.h>

CreateMenu::CreateMenu() {}

CreateMenu::~CreateMenu() {}

void CreateMenu::render(LandscapeGeneratorWindow& landscapeGeneratorWindow)
{
    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Landscape..."))
        {
            landscapeGeneratorWindow.open();
        }

        ImGui::EndMenu();
    }
}
