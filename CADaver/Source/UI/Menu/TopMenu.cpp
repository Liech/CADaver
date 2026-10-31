#include "UI/Menu/TopMenu.h"

#include "Database/Preferences/Preferences.h"
#include "Execution/Defer.h"
#include "Execution/GatedDatabase.h"
#include "UI/Preferences/PreferencesWindow.h"
#include "Vulcant/Toolkit/Camera.h"
#include "Windows/About/AboutWindow.h"
#include "Windows/About/CommandsWindow.h"
#include "Windows/About/WindowsWindow.h"
#include "Windows/Generator/LandscapeGeneratorWindow.h"

#include <imgui.h>

TopMenu::TopMenu()
  : preferencesWindow(std::make_unique<PreferencesWindow>())
  , aboutWindow(std::make_unique<AboutWindow>())
  , commandsWindow(std::make_unique<CommandsWindow>())
  , windowsWindow(std::make_unique<WindowsWindow>())
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
                    aboutWindow->open();
                }
            }
            if (ImGui::MenuItem("Commands"))
            {
                if (commandsWindow)
                {
                    commandsWindow->open();
                }
            }
            if (ImGui::MenuItem("Windows"))
            {
                if (windowsWindow)
                {
                    windowsWindow->open();
                }
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void TopMenu::renderWindows(GatedDatabase& gatedDatabase, UndoStack& undoStack, Vulcant::Toolkit::Camera* camera)
{
    auto& defer = Defer::getInstance();
    if (preferencesWindow)
    {
        preferencesWindow->render(gatedDatabase, undoStack, camera);
    }
    if (aboutWindow)
    {
        aboutWindow->setInput(gatedDatabase, defer);
        aboutWindow->render();
    }
    if (commandsWindow)
    {
        commandsWindow->setInput(gatedDatabase, defer);
        commandsWindow->render();
    }
    if (windowsWindow)
    {
        windowsWindow->setInput(gatedDatabase, defer);
        windowsWindow->render();
    }
    if (landscapeGeneratorWindow)
    {
        landscapeGeneratorWindow->setInput(gatedDatabase, defer);
        landscapeGeneratorWindow->render();
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

WindowsWindow* TopMenu::getWindowsWindow() const
{
    return windowsWindow.get();
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
