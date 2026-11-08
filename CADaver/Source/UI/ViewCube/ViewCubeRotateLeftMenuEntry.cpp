#include "UI/ViewCube/ViewCubeRotateLeftMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeRotateLeftMenuEntry::ViewCubeRotateLeftMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Rotate 90° Left", "View/Rotate Left", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeRotateLeftMenuEntry::~ViewCubeRotateLeftMenuEntry() {}

std::string ViewCubeRotateLeftMenuEntry::getType() const
{
    return "ViewRotateLeft";
}

std::string ViewCubeRotateLeftMenuEntry::getDescription() const
{
    return "Menu: View/Rotate Left\nRotates camera 90 degrees left around the current target.";
}

void ViewCubeRotateLeftMenuEntry::click()
{
    viewController.rotateLeft();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeRotateLeftMenuEntry/Construction")
{
    GatedDatabase               db;
    UndoStack                   us(db);
    Windows                     windows;
    ViewController              vc;
    ViewCubeRotateLeftMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewRotateLeft");
    REQUIRE(entry.getLabel() == "Rotate 90° Left");
    REQUIRE(entry.getPath() == "View/Rotate Left");
}
#endif
