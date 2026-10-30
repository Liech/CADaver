#include "Database.h"

Database::Database()
{
    preferences.load();
}

Database::~Database() {}

Project& Database::getProject()
{
    return project;
}

const Project& Database::getProject() const
{
    return project;
}

Preferences& Database::getPreferences()
{
    return preferences;
}

const Preferences& Database::getPreferences() const
{
    return preferences;
}

UndoStack& Database::getUndoStack()
{
    return undoStack;
}

const UndoStack& Database::getUndoStack() const
{
    return undoStack;
}
