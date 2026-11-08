#include "UIElement/Menu/File/LoadStepMenuEntry.h"

#include "Windows/Windows.h"

LoadStepMenuEntry::LoadStepMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("STEP...", "File/Load/STEP", "", gatedDatabase, undoStack, windows)
{
}

LoadStepMenuEntry::~LoadStepMenuEntry() {}

std::string LoadStepMenuEntry::getType() const
{
    return "STEP";
}

std::string LoadStepMenuEntry::getDescription() const
{
    return "Menu: File/Load/STEP\nImports a boundary representation (B-Rep / STEP) CAD file.";
}

void LoadStepMenuEntry::click()
{
    windows.open("LoadStepAsset");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStepMenuEntry/TypeAndClick")
{
    GatedDatabase     db;
    UndoStack         us(db);
    Windows           windows;
    LoadStepMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "STEP");
    REQUIRE(entry.getLabel() == "STEP...");
    REQUIRE(entry.getPath() == "File/Load/STEP");

    entry.click();
    REQUIRE(windows.getOpen().count("LoadStepAsset") == 1);
}
#endif
