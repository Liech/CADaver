#pragma once

#include "UI/Menu/CreateMenu.h"
#include "UI/Menu/EditMenu.h"
#include "UI/Menu/FileMenu.h"

#include <memory>

namespace Vulcant::Toolkit
{
    class Camera;
}

class AboutWindow;
class CommandsWindow;
class GatedDatabase;
class LandscapeGeneratorWindow;
class PreferencesWindow;

// Coordinates the main top menu bar by hosting File, Edit, Create, and Help menus and their dialog windows.
class TopMenu
{
  public:
    TopMenu();
    virtual ~TopMenu();

    void render(GatedDatabase& gatedDatabase, bool& quitRequested);
    void renderWindows(GatedDatabase& gatedDatabase, Vulcant::Toolkit::Camera* camera = nullptr);

    PreferencesWindow*        getPreferencesWindow() const;
    AboutWindow*              getAboutWindow() const;
    CommandsWindow*           getCommandsWindow() const;
    LandscapeGeneratorWindow* getLandscapeGeneratorWindow() const;

  private:
    FileMenu                                  fileMenu;
    EditMenu                                  editMenu;
    CreateMenu                                createMenu;
    std::unique_ptr<PreferencesWindow>        preferencesWindow;
    std::unique_ptr<AboutWindow>              aboutWindow;
    std::unique_ptr<CommandsWindow>           commandsWindow;
    std::unique_ptr<LandscapeGeneratorWindow> landscapeGeneratorWindow;
};
