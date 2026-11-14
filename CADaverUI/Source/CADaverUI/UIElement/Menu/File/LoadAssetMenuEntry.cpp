#include "UIElement/Menu/File/LoadAssetMenuEntry.h"

#include "Windows/Windows.h"

LoadAssetMenuEntry::LoadAssetMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows) : MenuEntry("Load...", "File/Load", "", gatedDatabase, undoStack, windows) {}

LoadAssetMenuEntry::~LoadAssetMenuEntry() {}

std::string LoadAssetMenuEntry::getType() const
{
    return "LoadAsset";
}

std::string LoadAssetMenuEntry::getDescription() const
{
    return "Menu: File/Load\nImports a CAD (STEP), mesh (STL), or voxel (SVO) asset file into the project.";
}

void LoadAssetMenuEntry::click()
{
    windows.open("LoadAsset");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("LoadAssetMenuEntry/TypeAndClick")
{
    GatedDatabase      db;
    UndoStack          us(db);
    Windows            windows;
    LoadAssetMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "LoadAsset");
    REQUIRE(entry.getLabel() == "Load...");
    REQUIRE(entry.getPath() == "File/Load");
    REQUIRE(entry.getShortcut().empty());
    REQUIRE(!entry.getDescription().empty());

    entry.click();
    REQUIRE(windows.getOpen().count("LoadAsset") == 1);
}
#endif
