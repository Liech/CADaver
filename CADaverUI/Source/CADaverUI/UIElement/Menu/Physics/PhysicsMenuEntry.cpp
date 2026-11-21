#include "UIElement/Menu/Physics/PhysicsMenuEntry.h"

#include "Windows/Windows.h"

PhysicsMenuEntry::PhysicsMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Physics Simulation...", "Physics/Physics Simulation", "", gatedDatabase, undoStack, windows)
{
}

PhysicsMenuEntry::~PhysicsMenuEntry() {}

std::string PhysicsMenuEntry::getType() const
{
    return "Physics";
}

std::string PhysicsMenuEntry::getDescription() const
{
    return "Menu: Physics/Physics Simulation\nOpens the Physics Simulation window to play and stop rigid body simulation.";
}

void PhysicsMenuEntry::click()
{
    windows.open("Physics");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PhysicsMenuEntry/TypeAndClick")
{
    GatedDatabase    db;
    UndoStack        us(db);
    Windows          windows;
    PhysicsMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Physics");
    REQUIRE(entry.getLabel() == "Physics Simulation...");
    REQUIRE(entry.getPath() == "Physics/Physics Simulation");

    entry.click();
    REQUIRE(windows.getOpen().count("Physics") == 1);
}
#endif
