#pragma once

#include <cstddef>
#include <string>

class Defer;
class UndoStack;

// Renders the undo/redo history and capacity metrics inside the Preferences dialog.
class UndoPreferencesView
{
  public:
    UndoPreferencesView();
    virtual ~UndoPreferencesView();

    bool render(UndoStack& undoStack, Defer& defer);

  private:
    std::string formatByteSize(size_t bytes) const;
};
