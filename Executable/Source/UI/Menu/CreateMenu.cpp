#include "UI/Menu/CreateMenu.h"

#include "UI/Generator/LandscapeGeneratorWindow.h"

#include <imgui.h>

CreateMenu::CreateMenu() {}

CreateMenu::~CreateMenu() {}

void CreateMenu::render(LandscapeGeneratorWindow& landscapeGeneratorWindow)
{
    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Landscape..."))
        {
            landscapeGeneratorWindow.setOpen(true);
        }

        ImGui::EndMenu();
    }
}
