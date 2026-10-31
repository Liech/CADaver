#pragma once

class Defer;
class UndoStack;

// Renders the Edit main menu containing Undo and Redo items with shortcuts.
class EditMenu
{
  public:
    EditMenu();
    virtual ~EditMenu();

    void render(UndoStack& undoStack, Defer& defer);

  private:
    void handleShortcuts(UndoStack& undoStack, Defer& defer);
    void triggerUndo(UndoStack& undoStack, Defer& defer);
    void triggerRedo(UndoStack& undoStack, Defer& defer);
};
