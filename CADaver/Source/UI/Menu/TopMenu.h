#pragma once

#include "UI/Menu/CreateMenu.h"
#include "UI/Menu/EditMenu.h"
#include "UI/Menu/FileMenu.h"

class Defer;
class GatedDatabase;
class UndoStack;
class Windows;

// Coordinates the main top menu bar by hosting File, Edit, Create, and Help menus.
class TopMenu
{
  public:
    TopMenu();
    virtual ~TopMenu();

    void render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Windows& windows, bool& quitRequested);

  private:
    FileMenu   fileMenu;
    EditMenu   editMenu;
    CreateMenu createMenu;
};
