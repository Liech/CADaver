#include "UI/ViewCube/ViewCubeHomeMenuEntry.h"

#include "Camera/ViewController.h"
#include "Execution/GatedDatabase.h"

ViewCubeHomeMenuEntry::ViewCubeHomeMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Home", "View/Home", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeHomeMenuEntry::~ViewCubeHomeMenuEntry() {}

std::string ViewCubeHomeMenuEntry::getType() const
{
    return "ViewHome";
}

std::string ViewCubeHomeMenuEntry::getDescription() const
{
    return "Menu: View/Home\nResets camera view to fit the current project geometry.";
}

void ViewCubeHomeMenuEntry::click()
{
    viewController.resetView(gatedDatabase);
}

#ifdef ISTESTPROJECT
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeHomeMenuEntry/Construction")
{
    GatedDatabase         db;
    UndoStack             us(db);
    Windows               windows;
    ViewController        vc;
    ViewCubeHomeMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewHome");
    REQUIRE(entry.getLabel() == "Home");
    REQUIRE(entry.getPath() == "View/Home");
}
#endif
