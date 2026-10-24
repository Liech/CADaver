#pragma once

#include "Preferences/Preferences.h"
#include "Project/Project.h"

// Headless singleton database holding project and application preferences. Must remain decoupled from rendering and UI.
class Database
{
  public:
    Database();
    virtual ~Database();

    static Database& getInstance();


    Project&       getProject();
    const Project& getProject() const;

    Preferences&       getPreferences();
    const Preferences& getPreferences() const;

  private:

    Project     project;
    Preferences preferences;
};
