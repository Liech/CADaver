#include "UI/Menu/TopMenu.h"

#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Windows.h"

#include <imgui.h>

TopMenu::TopMenu() {}

TopMenu::~TopMenu() {}

void TopMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Windows& windows, bool& quitRequested)
{
    searchBar.init(gatedDatabase, windows);

    if (ImGui::BeginMainMenuBar())
    {
        fileMenu.render(windows, quitRequested);
        editMenu.render(undoStack, defer);
        createMenu.render(windows);

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About CADaver"))
            {
                windows.open("About");
            }
            if (ImGui::MenuItem("Commands"))
            {
                windows.open("CommandsInfo");
            }
            if (ImGui::MenuItem("Concepts"))
            {
                windows.open("ConceptsInfo");
            }
            if (ImGui::MenuItem("Windows"))
            {
                windows.open("WindowsInfo");
            }
            ImGui::EndMenu();
        }

        ImGui::SameLine();
        ImGui::Spacing();
        ImGui::SameLine();

        searchBar.render(defer);

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
