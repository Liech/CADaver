#include "UI/ViewCube/ViewCubeToggleOrthographicMenuEntry.h"

#include "Camera/ViewController.h"

ViewCubeToggleOrthographicMenuEntry::ViewCubeToggleOrthographicMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Toggle Orthographic", "View/Toggle Orthographic", "", gatedDatabase, undoStack, windows)
  , viewController(viewController)
{
}

ViewCubeToggleOrthographicMenuEntry::~ViewCubeToggleOrthographicMenuEntry() {}

std::string ViewCubeToggleOrthographicMenuEntry::getType() const
{
    return "ViewToggleOrthographic";
}

std::string ViewCubeToggleOrthographicMenuEntry::getDescription() const
{
    return "Menu: View/Toggle Orthographic\nToggles viewport camera between perspective and orthographic projection mode.";
}

void ViewCubeToggleOrthographicMenuEntry::click()
{
    viewController.toggleProjection();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ViewCubeToggleOrthographicMenuEntry/Construction")
{
    GatedDatabase                       db;
    UndoStack                           us(db);
    Windows                             windows;
    ViewController                      vc;
    ViewCubeToggleOrthographicMenuEntry entry(vc, db, us, windows);

    REQUIRE(entry.getType() == "ViewToggleOrthographic");
    REQUIRE(entry.getLabel() == "Toggle Orthographic");
    REQUIRE(entry.getPath() == "View/Toggle Orthographic");
}
#endif
