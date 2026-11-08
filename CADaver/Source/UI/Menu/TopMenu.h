#pragma once

#include "Search/SearchBar.h"
#include "UIElement/Menu/DynamicMenuRenderer.h"
#include "UIElement/UIElementFactory.h"

class Defer;
class GatedDatabase;
class UndoStack;
class Windows;

// Coordinates the main top menu bar by dynamically rendering MenuEntry hierarchies alongside the search bar.
class TopMenu
{
  public:
    TopMenu();
    virtual ~TopMenu();

    void render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Defer& defer, Windows& windows, bool& quitRequested);
    void focusSearchBar();

    UIElementFactory&       getUIElementFactory();
    const UIElementFactory& getUIElementFactory() const;

  private:
    void handleShortcuts(UndoStack& undoStack);

    UIElementFactory    uiElementFactory;
    DynamicMenuRenderer dynamicMenuRenderer;
    SearchBar           searchBar;
    bool                factoryInitialized;
};
