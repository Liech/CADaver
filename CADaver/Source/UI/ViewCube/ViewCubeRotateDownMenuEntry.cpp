#include "UI/ViewCube/ViewCubeRotateDownMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeRotateDownMenuEntry::ViewCubeRotateDownMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Rotate 90° Down", "View/Rotate Down", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeRotateDownMenuEntry::~ViewCubeRotateDownMenuEntry() {}

std::string ViewCubeRotateDownMenuEntry::getType() const
{
    return "ViewRotateDown";
}

std::string ViewCubeRotateDownMenuEntry::getDescription() const
{
    return "Menu: View/Rotate Down\nRotates camera 90 degrees downward around the current target.";
}

void ViewCubeRotateDownMenuEntry::click()
{
    viewController.rotateDown();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeRotateDownMenuEntry/Construction")
{
    GatedDatabase               db;
    UndoStack                   us(db);
    Windows                     windows;
    ViewController              vc;
    ViewCubeRotateDownMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewRotateDown");
    REQUIRE(entry.getLabel() == "Rotate 90° Down");
    REQUIRE(entry.getPath() == "View/Rotate Down");
}
#endif
