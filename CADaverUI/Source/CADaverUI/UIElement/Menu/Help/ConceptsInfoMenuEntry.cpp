#include "UIElement/Menu/Help/ConceptsInfoMenuEntry.h"

#include "Windows/Windows.h"

ConceptsInfoMenuEntry::ConceptsInfoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry("Concepts", "Help/Concepts", "", gatedDatabase, undoStack, windows)
{
}

ConceptsInfoMenuEntry::~ConceptsInfoMenuEntry() {}

std::string ConceptsInfoMenuEntry::getType() const
{
    return "Concepts";
}

std::string ConceptsInfoMenuEntry::getDescription() const
{
    return "Menu: Help/Concepts\nOpens system concept definitions, domain models, and architecture documentation.";
}

void ConceptsInfoMenuEntry::click()
{
    windows.open("ConceptsInfo");
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConceptsInfoMenuEntry/TypeAndClick")
{
    GatedDatabase         db;
    UndoStack             us(db);
    Windows               windows;
    ConceptsInfoMenuEntry entry(db, us, windows);

    REQUIRE(entry.getType() == "Concepts");
    REQUIRE(entry.getLabel() == "Concepts");
    REQUIRE(entry.getPath() == "Help/Concepts");

    entry.click();
    REQUIRE(windows.getOpen().count("ConceptsInfo") == 1);
}
#endif
