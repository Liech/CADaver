#include "Database.h"

Database::Database() {}

Database::~Database() {}

Database& Database::getInstance()
{
    static Database instance;
    return instance;
}

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
