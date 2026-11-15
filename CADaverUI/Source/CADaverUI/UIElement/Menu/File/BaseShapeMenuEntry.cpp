#include "UIElement/Menu/File/BaseShapeMenuEntry.h"

#include "Execution/GatedDatabase.h"
#include "Windows/Windows.h"

BaseShapeMenuEntry::BaseShapeMenuEntry(const std::string& shapeId, const std::string& typeName, const std::string& label, const std::string& menuPath, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : MenuEntry(label, menuPath, "", gatedDatabase, undoStack, windows)
  , shapeId(shapeId)
  , typeName(typeName)
{
}

BaseShapeMenuEntry::~BaseShapeMenuEntry() {}

std::string BaseShapeMenuEntry::getType() const
{
    return typeName;
}

std::string BaseShapeMenuEntry::getDescription() const
{
    if (shapeId.empty())
    {
        return "Menu: " + getPath() + "\nOpens the Base Shape Generator dialog.";
    }
    return "Menu: " + getPath() + "\nGenerates a new " + getLabel() + " triangulation asset.";
}

void BaseShapeMenuEntry::click()
{
    if (shapeId.empty())
    {
        windows.open("BaseShapeGenerator");
        return;
    }

    Polyglot::Node command = {
        {    "type", "GenerateBaseShape" },
        { "version",                   1 },
        {   "shape",             shapeId }
    };
    gatedDatabase.executeAsync(command);
}

#ifdef ISTESTPROJECT
#include "Execution/GatedDatabase.h"
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("BaseShapeMenuEntry/DialogEntryClick")
{
    GatedDatabase      db;
    UndoStack          us(db);
    Windows            windows;
    BaseShapeMenuEntry entry("", "NewShapeGenerator", "Shape Generator...", "File/New/Shape Generator...", db, us, windows);

    REQUIRE(entry.getType() == "NewShapeGenerator");
    REQUIRE(entry.getLabel() == "Shape Generator...");
    REQUIRE(entry.getPath() == "File/New/Shape Generator...");

    entry.click();
    REQUIRE(windows.getOpen().count("BaseShapeGenerator") == 1);
}

TEST_CASE("BaseShapeMenuEntry/PresetEntryMetadata")
{
    GatedDatabase      db;
    UndoStack          us(db);
    Windows            windows;
    BaseShapeMenuEntry entry("cube", "NewCube", "Cube", "File/New/Cube", db, us, windows);

    REQUIRE(entry.getType() == "NewCube");
    REQUIRE(entry.getLabel() == "Cube");
    REQUIRE(entry.getPath() == "File/New/Cube");
    REQUIRE(!entry.getDescription().empty());
}
#endif
