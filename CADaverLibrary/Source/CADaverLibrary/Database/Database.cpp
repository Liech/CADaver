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

double Database::getCurrentTime() const
{
    return currentTime;
}

void Database::setCurrentTime(double time)
{
    currentTime = time;
}

#ifdef ISTESTPROJECT
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Database/DefaultState")
{
    Database db;
    CHECK(db.getProject().getAssetCount() == 0);
    CHECK(db.getSelection().getEntries().empty());
    CHECK(db.getCurrentTime() == 0.0);

    db.setCurrentTime(1.5);
    CHECK(db.getCurrentTime() == 1.5);
}
#endif
