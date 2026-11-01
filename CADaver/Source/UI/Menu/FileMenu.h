#pragma once

#include "UIElement/NullUIElement.h"
#include "UIElement/UIElement.h"

#include <map>
#include <memory>
#include <string>

class GatedDatabase;
class UndoStack;
class Windows;

// Renders the main file menu and opens file load and preferences windows.
class FileMenu
{
  public:
    FileMenu();
    virtual ~FileMenu();

    void init(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested);
    void render(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested);

    UIElement& get(const std::string& type);

  private:
    std::map<std::string, std::shared_ptr<UIElement>> entries;
    NullUIElement                                     fallbackElement;
    bool                                              initialized;
};
