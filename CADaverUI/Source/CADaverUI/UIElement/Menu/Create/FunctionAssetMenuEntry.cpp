#include "UIElement/Menu/Create/FunctionAssetMenuEntry.h"

#include "Windows/Windows.h"

FunctionAssetMenuEntry::FunctionAssetMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Function Asset...", "Create/Function Asset", "", gatedDatabase, undoStack, windows)
{
}

FunctionAssetMenuEntry::~FunctionAssetMenuEntry() {}

std::string FunctionAssetMenuEntry::getType() const
{
    return "FunctionAsset";
}

std::string FunctionAssetMenuEntry::getDescription() const
{
    return "Menu: Create/Function Asset\nOpens the function asset generator to create an SDF or SVO asset from a Slang formula.";
}

void FunctionAssetMenuEntry::click()
{
    windows.open("FunctionAssetGenerator");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FunctionAssetMenuEntry/TypeAndClick")
{
    GatedDatabase          db;
    UndoStack              us(db);
    Windows                windows;
    FunctionAssetMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "FunctionAsset");
    REQUIRE(entry.getLabel() == "Function Asset...");
    REQUIRE(entry.getPath() == "Create/Function Asset");

    entry.click();
    REQUIRE(windows.getOpen().count("FunctionAssetGenerator") == 1);
}
#endif
