#include "UIElement/Menu/WindowsInfoMenuEntry.h"

#include "Windows/Windows.h"

WindowsInfoMenuEntry::WindowsInfoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Windows", "Help/Windows", "", gatedDatabase, undoStack, windows)
{
}

WindowsInfoMenuEntry::~WindowsInfoMenuEntry() {}

std::string WindowsInfoMenuEntry::getType() const
{
    return "Windows";
}

std::string WindowsInfoMenuEntry::getDescription() const
{
    return "Menu: Help/Windows\nOpens an overview of all system windows, modals, and dockable views.";
}

void WindowsInfoMenuEntry::click()
{
    windows.open("WindowsInfo");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WindowsInfoMenuEntry/TypeAndClick")
{
    GatedDatabase        db;
    UndoStack            us(db);
    Windows              windows;
    WindowsInfoMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Windows");
    REQUIRE(entry.getLabel() == "Windows");
    REQUIRE(entry.getPath() == "Help/Windows");

    entry.click();
    REQUIRE(windows.getOpen().count("WindowsInfo") == 1);
}
#endif
