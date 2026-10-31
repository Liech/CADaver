#include "UI/Menu/TopMenu.h"

#include "Database/Preferences/Preferences.h"
#include "Execution/GatedDatabase.h"
#include "UI/About/AboutWindow.h"
#include "UI/Commands/CommandsWindow.h"
#include "UI/Generator/LandscapeGeneratorWindow.h"
#include "UI/Preferences/PreferencesWindow.h"
#include "Vulcant/Toolkit/Camera.h"

#include <imgui.h>

TopMenu::TopMenu()
  : preferencesWindow(std::make_unique<PreferencesWindow>())
  , aboutWindow(std::make_unique<AboutWindow>())
  , commandsWindow(std::make_unique<CommandsWindow>())
  , landscapeGeneratorWindow(std::make_unique<LandscapeGeneratorWindow>())
{
}

TopMenu::~TopMenu() {}

void TopMenu::render(GatedDatabase& gatedDatabase, UndoStack& undoStack, bool& quitRequested)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (preferencesWindow)
        {
            fileMenu.render(gatedDatabase, *preferencesWindow, quitRequested);
        }
        editMenu.render(undoStack);
        if (landscapeGeneratorWindow)
        {
            createMenu.render(*landscapeGeneratorWindow);
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About CADaver"))
            {
                if (aboutWindow)
                {
                    aboutWindow->setOpen(true);
                }
            }
            if (ImGui::MenuItem("Commands"))
            {
                if (commandsWindow)
                {
                    commandsWindow->setOpen(true);
                }
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
    if (aboutWindow)
    {
        aboutWindow->render();
    }
    if (commandsWindow)
    {
        commandsWindow->render(gatedDatabase);
    }
    if (landscapeGeneratorWindow)
    {
        landscapeGeneratorWindow->render(gatedDatabase);
    }
}

PreferencesWindow* TopMenu::getPreferencesWindow() const
{
    return preferencesWindow.get();
}

AboutWindow* TopMenu::getAboutWindow() const
{
    return aboutWindow.get();
}

CommandsWindow* TopMenu::getCommandsWindow() const
{
    return commandsWindow.get();
}

LandscapeGeneratorWindow* TopMenu::getLandscapeGeneratorWindow() const
{
    return landscapeGeneratorWindow.get();
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TopMenu/Construction")
{
    TopMenu topMenu;
    CHECK(topMenu.getPreferencesWindow() != nullptr);
    CHECK(topMenu.getAboutWindow() != nullptr);
    CHECK(topMenu.getCommandsWindow() != nullptr);
    CHECK(topMenu.getLandscapeGeneratorWindow() != nullptr);
}
#endif
