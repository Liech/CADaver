#include "UIElement/Menu/Edit/UndoMenuEntry.h"

#include "Execution/UndoStack.h"

UndoMenuEntry::UndoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Undo", "Edit/Undo", "Ctrl+Z", gatedDatabase, undoStack, windows)
{
    setEnabled(undoStack.canUndo());
}

UndoMenuEntry::~UndoMenuEntry() {}

std::string UndoMenuEntry::getType() const
{
    return "Undo";
}

std::string UndoMenuEntry::getDescription() const
{
    return "Menu: Edit/Undo\nReverts the last performed action or modification.";
}

bool UndoMenuEntry::render()
{
    setEnabled(undoStack.canUndo());
    return MenuEntry::render();
}

void UndoMenuEntry::click()
{
    undoStack.undo();
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UndoMenuEntry/TypeAndClick")
{
    GatedDatabase db;
    UndoStack     undoStack(db);
    Windows       windows;
    UndoMenuEntry entry(db, undoStack, windows);

    REQUIRE(entry.getType() == "Undo");
    REQUIRE(entry.getLabel() == "Undo");
    REQUIRE(entry.getPath() == "Edit/Undo");
    REQUIRE(entry.getShortcut() == "Ctrl+Z");
    REQUIRE_FALSE(entry.isEnabled());
}
#endif
