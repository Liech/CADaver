#include "UI/ViewCube/ViewCubeRotateUpMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeRotateUpMenuEntry::ViewCubeRotateUpMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Rotate 90° Up", "View/Rotate Up", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeRotateUpMenuEntry::~ViewCubeRotateUpMenuEntry() {}

std::string ViewCubeRotateUpMenuEntry::getType() const
{
    return "ViewRotateUp";
}

std::string ViewCubeRotateUpMenuEntry::getDescription() const
{
    return "Menu: View/Rotate Up\nRotates camera 90 degrees upward around the current target.";
}

void ViewCubeRotateUpMenuEntry::click()
{
    viewController.rotateUp();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeRotateUpMenuEntry/Construction")
{
    GatedDatabase             db;
    UndoStack                 us(db);
    Windows                   windows;
    ViewController            vc;
    ViewCubeRotateUpMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewRotateUp");
    REQUIRE(entry.getLabel() == "Rotate 90° Up");
    REQUIRE(entry.getPath() == "View/Rotate Up");
}
#endif
