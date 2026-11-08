#include "UI/ViewCube/ViewCubeRotateRightMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeRotateRightMenuEntry::ViewCubeRotateRightMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Rotate 90° Right", "View/Rotate Right", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeRotateRightMenuEntry::~ViewCubeRotateRightMenuEntry() {}

std::string ViewCubeRotateRightMenuEntry::getType() const
{
    return "ViewRotateRight";
}

std::string ViewCubeRotateRightMenuEntry::getDescription() const
{
    return "Menu: View/Rotate Right\nRotates camera 90 degrees right around the current target.";
}

void ViewCubeRotateRightMenuEntry::click()
{
    viewController.rotateRight();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeRotateRightMenuEntry/Construction")
{
    GatedDatabase                db;
    UndoStack                    us(db);
    Windows                      windows;
    ViewController               vc;
    ViewCubeRotateRightMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewRotateRight");
    REQUIRE(entry.getLabel() == "Rotate 90° Right");
    REQUIRE(entry.getPath() == "View/Rotate Right");
}
#endif
