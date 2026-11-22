#include "UIElement/Menu/Create/FunctionSvoMenuEntry.h"

#include "Windows/Windows.h"

FunctionSvoMenuEntry::FunctionSvoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Function SVO...", "Create/Function SVO", "", gatedDatabase, undoStack, windows)
{
}

FunctionSvoMenuEntry::~FunctionSvoMenuEntry() {}

std::string FunctionSvoMenuEntry::getType() const
{
    return "FunctionSvo";
}

std::string FunctionSvoMenuEntry::getDescription() const
{
    return "Menu: Create/Function SVO\nOpens the Lua function SVO generator with sample presets and a custom script editor.";
}

void FunctionSvoMenuEntry::click()
{
    windows.open("FunctionSvoGenerator");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FunctionSvoMenuEntry/TypeAndClick")
{
    GatedDatabase        db;
    UndoStack            us(db);
    Windows              windows;
    FunctionSvoMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "FunctionSvo");
    REQUIRE(entry.getLabel() == "Function SVO...");
    REQUIRE(entry.getPath() == "Create/Function SVO");

    entry.click();
    REQUIRE(windows.getOpen().count("FunctionSvoGenerator") == 1);
}
#endif
