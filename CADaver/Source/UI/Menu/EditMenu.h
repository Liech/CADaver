#pragma once

class UndoStack;

// Renders the Edit main menu containing Undo and Redo items with shortcuts.
class EditMenu
{
  public:
    EditMenu();
    virtual ~EditMenu();

    void render();

  private:
    void handleShortcuts(const UndoStack& undoStack);
    void triggerUndo();
    void triggerRedo();
};
