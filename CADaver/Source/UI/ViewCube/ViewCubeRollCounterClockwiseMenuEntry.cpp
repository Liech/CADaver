#include "UI/ViewCube/ViewCubeRollCounterClockwiseMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeRollCounterClockwiseMenuEntry::ViewCubeRollCounterClockwiseMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Roll Counter-Clockwise", "View/Roll Counter-Clockwise", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeRollCounterClockwiseMenuEntry::~ViewCubeRollCounterClockwiseMenuEntry() {}

std::string ViewCubeRollCounterClockwiseMenuEntry::getType() const
{
    return "ViewRollCounterClockwise";
}

std::string ViewCubeRollCounterClockwiseMenuEntry::getDescription() const
{
    return "Menu: View/Roll Counter-Clockwise\nRolls viewport camera 90 degrees counter-clockwise around the viewing line.";
}

void ViewCubeRollCounterClockwiseMenuEntry::click()
{
    viewController.rollCcw();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeRollCounterClockwiseMenuEntry/Construction")
{
    GatedDatabase                         db;
    UndoStack                             us(db);
    Windows                               windows;
    ViewController                        vc;
    ViewCubeRollCounterClockwiseMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewRollCounterClockwise");
    REQUIRE(entry.getLabel() == "Roll Counter-Clockwise");
    REQUIRE(entry.getPath() == "View/Roll Counter-Clockwise");
}
#endif
