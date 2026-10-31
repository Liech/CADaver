#include "UI/Menu/TopMenu.h"

#include "Database/Preferences/Preferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Preferences/PreferencesWindow.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Windows/Windows.h"

#include <imgui.h>

TopMenu::TopMenu()
  : preferencesWindow(std::make_unique<PreferencesWindow>())
{
}

TopMenu::~TopMenu() {}

void TopMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Windows& windows, bool& quitRequested)
{
    (void)gatedDatabase;
    if (ImGui::BeginMainMenuBar())
    {
        if (preferencesWindow)
        {
            fileMenu.render(windows, *preferencesWindow, quitRequested);
        }
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

void TopMenu::renderWindows(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Vulcant::Toolkit::Camera* camera)
{
    if (preferencesWindow)
    {
        preferencesWindow->render(gatedDatabase, undoStack, defer, camera);
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
