#pragma once

#include <cstddef>
#include <string>

class GatedDatabase;
class UndoStack;

// Renders the undo/redo history and capacity metrics inside the Preferences dialog.
class UndoPreferencesView
{
  public:
    UndoPreferencesView();
    virtual ~UndoPreferencesView();

    bool render(GatedDatabase& gatedDatabase, const UndoStack& undoStack);

  private:
    std::string formatByteSize(size_t bytes) const;
};
