#pragma once

class UndoStack;

// Renders the Edit main menu containing Undo and Redo items with shortcuts.
class EditMenu
{
  public:
    EditMenu();
    virtual ~EditMenu();

    void render(UndoStack& undoStack);

  private:
    void handleShortcuts(UndoStack& undoStack);
    void triggerUndo(UndoStack& undoStack);
    void triggerRedo(UndoStack& undoStack);
};
