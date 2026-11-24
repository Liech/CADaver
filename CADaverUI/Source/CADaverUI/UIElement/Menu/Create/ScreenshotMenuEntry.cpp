#include "UIElement/Menu/Create/ScreenshotMenuEntry.h"

#include "Execution/GatedDatabase.h"

ScreenshotMenuEntry::ScreenshotMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Screenshot", "Create/Screenshot", "", gatedDatabase, undoStack, windows)
{
}

ScreenshotMenuEntry::~ScreenshotMenuEntry() {}

std::string ScreenshotMenuEntry::getType() const
{
    return "Screenshot";
}

std::string ScreenshotMenuEntry::getDescription() const
{
    return "Menu: Create/Screenshot\nCaptures the current viewport image and adds it to the project as a Texture asset.";
}

void ScreenshotMenuEntry::click()
{
    Polyglot::Node command = {
        {    "type", "CaptureScreenshot" },
        { "version",                   1 }
    };
    gatedDatabase.execute(command);
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ScreenshotMenuEntry/TypeAndPath")
{
    GatedDatabase       db;
    UndoStack           us(db);
    Windows             windows;
    ScreenshotMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Screenshot");
    REQUIRE(entry.getLabel() == "Screenshot");
    REQUIRE(entry.getPath() == "Create/Screenshot");
}
#endif
