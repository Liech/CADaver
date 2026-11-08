#include "UIElement/Menu/File/LoadStlMenuEntry.h"

#include "Windows/Windows.h"

LoadStlMenuEntry::LoadStlMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("STL...", "File/Load/STL", "", gatedDatabase, undoStack, windows)
{
}

LoadStlMenuEntry::~LoadStlMenuEntry() {}

std::string LoadStlMenuEntry::getType() const
{
    return "STL";
}

std::string LoadStlMenuEntry::getDescription() const
{
    return "Menu: File/Load/STL\nImports a stereolithography (STL) mesh model into the project.";
}

void LoadStlMenuEntry::click()
{
    windows.open("LoadStlAsset");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadStlMenuEntry/TypeAndClick")
{
    GatedDatabase    db;
    UndoStack        us(db);
    Windows          windows;
    LoadStlMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "STL");
    REQUIRE(entry.getLabel() == "STL...");
    REQUIRE(entry.getPath() == "File/Load/STL");

    entry.click();
    REQUIRE(windows.getOpen().count("LoadStlAsset") == 1);
}
#endif
