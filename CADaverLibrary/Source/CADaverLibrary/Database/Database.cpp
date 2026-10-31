#include "Database/Database.h"

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

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Database/DefaultState")
{
    Database db;
    CHECK(db.getProject().getAssetCount() == 0);
}
#endif
