#pragma once

#include "UI/Menu/CreateMenu.h"
#include "UI/Menu/EditMenu.h"
#include "UI/Menu/FileMenu.h"

#include <memory>

namespace Vulcant::Toolkit
{
    class Camera;
}

class GatedDatabase;
class PreferencesWindow;
class UndoStack;
class WindowContainer;

// Coordinates the main top menu bar by hosting File, Edit, Create, and Help menus and their dialog windows.
class TopMenu
{
  public:
    TopMenu();
    virtual ~TopMenu();

    void render(GatedDatabase& gatedDatabase, UndoStack& undoStack, WindowContainer& windowContainer, bool& quitRequested);
    void renderWindows(GatedDatabase& gatedDatabase, UndoStack& undoStack, Vulcant::Toolkit::Camera* camera = nullptr);

    PreferencesWindow* getPreferencesWindow() const;

  private:
    FileMenu                           fileMenu;
    EditMenu                           editMenu;
    CreateMenu                         createMenu;
    std::unique_ptr<PreferencesWindow> preferencesWindow;
};
