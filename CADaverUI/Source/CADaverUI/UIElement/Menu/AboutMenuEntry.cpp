#include "UIElement/Menu/AboutMenuEntry.h"

#include "Windows/Windows.h"

AboutMenuEntry::AboutMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("About CADaver", "Help/About CADaver", "", gatedDatabase, undoStack, windows)
{
}

AboutMenuEntry::~AboutMenuEntry() {}

std::string AboutMenuEntry::getType() const
{
    return "About";
}

std::string AboutMenuEntry::getDescription() const
{
    return "Menu: Help/About CADaver\nDisplays build details, version metadata, and third-party license notices.";
}

void AboutMenuEntry::click()
{
    windows.open("About");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("AboutMenuEntry/TypeAndClick")
{
    GatedDatabase  db;
    UndoStack      us(db);
    Windows        windows;
    AboutMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "About");
    REQUIRE(entry.getLabel() == "About CADaver");
    REQUIRE(entry.getPath() == "Help/About CADaver");

    entry.click();
    REQUIRE(windows.getOpen().count("About") == 1);
}
#endif
