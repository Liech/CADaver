#include "UI/Menu/TopMenu.h"

#include "Database/Preferences/Preferences.h"
#include "Execution/GatedDatabase.h"
#include "UI/Preferences/PreferencesWindow.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Windows/WindowContainer.h"

#include <imgui.h>

TopMenu::TopMenu()
  : preferencesWindow(std::make_unique<PreferencesWindow>())
{
}

TopMenu::~TopMenu() {}

void TopMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, WindowContainer& windowContainer, bool& quitRequested)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (preferencesWindow)
        {
            fileMenu.render(gatedDatabase, *preferencesWindow, quitRequested);
        }
        editMenu.render(undoStack);
        createMenu.render(windowContainer);

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About CADaver"))
            {
                windowContainer.open("AboutWindow");
            }
            if (ImGui::MenuItem("Commands"))
            {
                windowContainer.open("CommandsWindow");
            }
            if (ImGui::MenuItem("Windows"))
            {
                windowContainer.open("WindowsWindow");
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void TopMenu::renderWindows(GatedDatabase& gatedDatabase, UndoStack& undoStack, Vulcant::Toolkit::Camera* camera)
{
    if (preferencesWindow)
    {
        preferencesWindow->render(gatedDatabase, undoStack, camera);
    }
}

PreferencesWindow* TopMenu::getPreferencesWindow() const
{
    return preferencesWindow.get();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TopMenu/Construction")
{
    TopMenu topMenu;
    CHECK(topMenu.getPreferencesWindow() != nullptr);
}
#endif
