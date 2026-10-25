#pragma once

#include "Preferences/Preferences.h"
#include "Project/Project.h"

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

  private:
    Project     project;
    Preferences preferences;
};
