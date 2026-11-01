#include "UIElement/Menu/File/LoadSvoMenuEntry.h"

#include "Windows/Windows.h"

LoadSvoMenuEntry::LoadSvoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("SVO...", "File/Load/SVO", "", gatedDatabase, undoStack, windows)
{
}

LoadSvoMenuEntry::~LoadSvoMenuEntry() {}

std::string LoadSvoMenuEntry::getType() const
{
    return "SVO";
}

std::string LoadSvoMenuEntry::getDescription() const
{
    return "Menu: File/Load/SVO\nLoads an SVO voxel file into the workspace.";
}

void LoadSvoMenuEntry::click()
{
    windows.open("LoadSvoAsset");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadSvoMenuEntry/TypeAndClick")
{
    GatedDatabase    db;
    UndoStack        us(db);
    Windows          windows;
    LoadSvoMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "SVO");
    REQUIRE(entry.getLabel() == "SVO...");
    REQUIRE(entry.getPath() == "File/Load/SVO");

    entry.click();
    REQUIRE(windows.getOpen().count("LoadSvoAsset") == 1);
}
#endif
