#include "UI/Menu/TopMenu.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Windows.h"

#include <imgui.h>

TopMenu::TopMenu() {}

TopMenu::~TopMenu() {}

void TopMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Windows& windows, bool& quitRequested)
{
    (void)gatedDatabase;
    if (ImGui::BeginMainMenuBar())
    {
        fileMenu.render(windows, quitRequested);
        editMenu.render(undoStack, defer);
        createMenu.render(windows);

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About CADaver"))
            {
                windows.open("AboutWindow");
            }
            if (ImGui::MenuItem("Commands"))
            {
                windows.open("CommandsInfoWindow");
            }
            if (ImGui::MenuItem("Windows"))
            {
                windows.open("WindowsInfoWindow");
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TopMenu/Construction")
{
    TopMenu topMenu;
}
#endif
