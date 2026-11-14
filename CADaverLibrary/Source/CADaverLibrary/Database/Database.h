#pragma once

#include "Database/Preferences/Preferences.h"
#include "Database/Project.h"
#include "Database/Selection/Selection.h"

// Headless database holding project, preferences, and selection, guarded by GatedDatabase.
class Database
{
  public:
    Database();
    virtual ~Database();

    Project&       getProject();
    const Project& getProject() const;

    Preferences&       getPreferences();
    const Preferences& getPreferences() const;

    Selection&       getSelection();
    const Selection& getSelection() const;

  private:
    Project     project;
    Preferences preferences;
    Selection   selection;
};
