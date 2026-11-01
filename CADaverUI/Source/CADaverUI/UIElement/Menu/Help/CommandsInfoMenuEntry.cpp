#include "UIElement/Menu/Help/CommandsInfoMenuEntry.h"

#include "Windows/Windows.h"

CommandsInfoMenuEntry::CommandsInfoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Commands", "Help/Commands", "", gatedDatabase, undoStack, windows)
{
}

CommandsInfoMenuEntry::~CommandsInfoMenuEntry() {}

std::string CommandsInfoMenuEntry::getType() const
{
    return "Commands";
}

std::string CommandsInfoMenuEntry::getDescription() const
{
    return "Menu: Help/Commands\nOpens the command documentation and registry viewer.";
}

void CommandsInfoMenuEntry::click()
{
    windows.open("CommandsInfo");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CommandsInfoMenuEntry/TypeAndClick")
{
    GatedDatabase         db;
    UndoStack             us(db);
    Windows               windows;
    CommandsInfoMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Commands");
    REQUIRE(entry.getLabel() == "Commands");
    REQUIRE(entry.getPath() == "Help/Commands");

    entry.click();
    REQUIRE(windows.getOpen().count("CommandsInfo") == 1);
}
#endif
