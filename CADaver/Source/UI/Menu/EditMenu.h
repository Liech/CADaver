#pragma once

class GatedDatabase;
class UndoStack;

// Renders the Edit main menu containing Undo and Redo items with shortcuts.
class EditMenu
{
  public:
    EditMenu();
    virtual ~EditMenu();

    void render(GatedDatabase& gatedDatabase);

  private:
    void handleShortcuts(GatedDatabase& gatedDatabase, const UndoStack& undoStack);
    void triggerUndo(GatedDatabase& gatedDatabase);
    void triggerRedo(GatedDatabase& gatedDatabase);
};
