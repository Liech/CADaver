#pragma once

#include "Database/Physics/PhysicsSimulation.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Project.h"
#include "Database/Selection/Selection.h"

// Headless database holding project, preferences, selection, and physics simulation, guarded by GatedDatabase.
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

    PhysicsSimulation&       getPhysicsSimulation();
    const PhysicsSimulation& getPhysicsSimulation() const;

  private:
    Project           project;
    Preferences       preferences;
    Selection         selection;
    PhysicsSimulation physicsSimulation;
};
