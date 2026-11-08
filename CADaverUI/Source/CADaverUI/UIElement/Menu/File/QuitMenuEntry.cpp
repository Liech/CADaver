#include "UIElement/Menu/File/QuitMenuEntry.h"

QuitMenuEntry::QuitMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested)
  : MenuEntry("Quit", "File/Quit", "Alt+F4", gatedDatabase, undoStack, windows)
  , quitRequested(quitRequested)
{
    setSeparatorBefore(true);
}

QuitMenuEntry::~QuitMenuEntry() {}

std::string QuitMenuEntry::getType() const
{
    return "Quit";
}

std::string QuitMenuEntry::getDescription() const
{
    return "Menu: File/Quit\nExits the CADaver application.";
}

void QuitMenuEntry::click()
{
    quitRequested = true;
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("QuitMenuEntry/TypeAndClick")
{
    GatedDatabase db;
    UndoStack     us(db);
    Windows       windows;
    bool          quitFlag = false;
    QuitMenuEntry entry(db, us, windows, quitFlag);

    REQUIRE(entry.getType() == "Quit");
    REQUIRE(entry.getLabel() == "Quit");
    REQUIRE(entry.getPath() == "File/Quit");
    REQUIRE(entry.getShortcut() == "Alt+F4");

    entry.click();
    REQUIRE(quitFlag);
}
#endif
