#include "UIElement/Menu/Create/LandscapeMenuEntry.h"

#include "Windows/Windows.h"

LandscapeMenuEntry::LandscapeMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Landscape...", "Create/Landscape", "", gatedDatabase, undoStack, windows)
{
}

LandscapeMenuEntry::~LandscapeMenuEntry() {}

std::string LandscapeMenuEntry::getType() const
{
    return "Landscape";
}

std::string LandscapeMenuEntry::getDescription() const
{
    return "Menu: Create/Landscape\nOpens procedural terrain generation using Simplex noise, FBM, and hydraulic erosion.";
}

void LandscapeMenuEntry::click()
{
    windows.open("LandscapeGenerator");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LandscapeMenuEntry/TypeAndClick")
{
    GatedDatabase      db;
    UndoStack          us(db);
    Windows            windows;
    LandscapeMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Landscape");
    REQUIRE(entry.getLabel() == "Landscape...");
    REQUIRE(entry.getPath() == "Create/Landscape");

    entry.click();
    REQUIRE(windows.getOpen().count("LandscapeGenerator") == 1);
}
#endif
