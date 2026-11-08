#include "UI/ViewCube/ViewCubeRollClockwiseMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeRollClockwiseMenuEntry::ViewCubeRollClockwiseMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Roll Clockwise", "View/Roll Clockwise", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeRollClockwiseMenuEntry::~ViewCubeRollClockwiseMenuEntry() {}

std::string ViewCubeRollClockwiseMenuEntry::getType() const
{
    return "ViewRollClockwise";
}

std::string ViewCubeRollClockwiseMenuEntry::getDescription() const
{
    return "Menu: View/Roll Clockwise\nRolls viewport camera 90 degrees clockwise around the viewing line.";
}

void ViewCubeRollClockwiseMenuEntry::click()
{
    viewController.rollCw();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeRollClockwiseMenuEntry/Construction")
{
    GatedDatabase                  db;
    UndoStack                      us(db);
    Windows                        windows;
    ViewController                 vc;
    ViewCubeRollClockwiseMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewRollClockwise");
    REQUIRE(entry.getLabel() == "Roll Clockwise");
    REQUIRE(entry.getPath() == "View/Roll Clockwise");
}
#endif
