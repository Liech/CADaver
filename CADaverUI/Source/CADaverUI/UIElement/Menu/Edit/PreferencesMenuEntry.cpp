#include "UIElement/Menu/Edit/PreferencesMenuEntry.h"

#include "Windows/Windows.h"

PreferencesMenuEntry::PreferencesMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Preferences...", "File/Preferences", "", gatedDatabase, undoStack, windows)
{
    setSeparatorBefore(true);
}

PreferencesMenuEntry::~PreferencesMenuEntry() {}

std::string PreferencesMenuEntry::getType() const
{
    return "Preferences";
}

std::string PreferencesMenuEntry::getDescription() const
{
    return "Menu: File/Preferences\nOpens global configuration settings for UI, rendering, camera, and SVO.";
}

void PreferencesMenuEntry::click()
{
    windows.open("Preferences");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("PreferencesMenuEntry/TypeAndClick")
{
    GatedDatabase        db;
    UndoStack            us(db);
    Windows              windows;
    PreferencesMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Preferences");
    REQUIRE(entry.getLabel() == "Preferences...");
    REQUIRE(entry.getPath() == "File/Preferences");

    entry.click();
    REQUIRE(windows.getOpen().count("Preferences") == 1);
}
#endif
