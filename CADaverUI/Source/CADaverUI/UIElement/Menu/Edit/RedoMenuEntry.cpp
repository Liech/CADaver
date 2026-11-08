#include "UIElement/Menu/Edit/RedoMenuEntry.h"

#include "Execution/UndoStack.h"

RedoMenuEntry::RedoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Redo", "Edit/Redo", "Ctrl+Y", gatedDatabase, undoStack, windows)
{
    setEnabled(undoStack.canRedo());
}

RedoMenuEntry::~RedoMenuEntry() {}

std::string RedoMenuEntry::getType() const
{
    return "Redo";
}

std::string RedoMenuEntry::getDescription() const
{
    return "Menu: Edit/Redo\nReapplies the previously reverted action.";
}

bool RedoMenuEntry::render()
{
    setEnabled(undoStack.canRedo());
    return MenuEntry::render();
}

void RedoMenuEntry::click()
{
    undoStack.redo();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("RedoMenuEntry/TypeAndClick")
{
    GatedDatabase db;
    UndoStack     undoStack(db);
    Windows       windows;
    RedoMenuEntry entry(db, undoStack, windows);

    REQUIRE(entry.getType() == "Redo");
    REQUIRE(entry.getLabel() == "Redo");
    REQUIRE(entry.getPath() == "Edit/Redo");
    REQUIRE(entry.getShortcut() == "Ctrl+Y");
    REQUIRE_FALSE(entry.isEnabled());
}
#endif
