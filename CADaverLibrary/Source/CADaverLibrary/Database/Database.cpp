#include "Database/Database.h"

Database::Database() {}

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

Selection& Database::getSelection()
{
    return selection;
}

const Selection& Database::getSelection() const
{
    return selection;
}

PhysicsSimulation& Database::getPhysicsSimulation()
{
    return physicsSimulation;
}

const PhysicsSimulation& Database::getPhysicsSimulation() const
{
    return physicsSimulation;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Database/DefaultState")
{
    Database db;
    CHECK(db.getProject().getAssetCount() == 0);
    CHECK(db.getSelection().getEntries().empty());
}
#endif
