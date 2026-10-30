#pragma once

#include "Commands/Undo/UndoStack.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Project.h"

// Headless database holding project and application preferences, guarded by GatedDatabase.
class Database
{
  public:
    Database();
    virtual ~Database();

    Project&       getProject();
    const Project& getProject() const;

    Preferences&       getPreferences();
    const Preferences& getPreferences() const;

    UndoStack&       getUndoStack();
    const UndoStack& getUndoStack() const;

  private:
    Project     project;
    Preferences preferences;
    UndoStack   undoStack;
};
