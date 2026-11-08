#include "UIElement/Radio/ColorThemeRadioElement.h"

#include "Database/Database.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/UIPreferences.h"
#include "Execution/GatedDatabase.h"
#include "Windows/Preferences/UIPreferencesTab.h"

ColorThemeRadioElement::ColorThemeRadioElement(int targetTheme, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : RadioUIElement("Color Theme", getThemeLabel(targetTheme), gatedDatabase, undoStack, windows)
  , targetTheme(targetTheme)
{
}

ColorThemeRadioElement::~ColorThemeRadioElement() {}

std::string ColorThemeRadioElement::getDescription() const
{
    switch (targetTheme)
    {
        case 1:
            return "User interface color theme option: Light\nApplies the light visual theme to all windows and controls.";
        case 2:
            return "User interface color theme option: Classic\nApplies the classic retro visual theme to all windows and controls.";
        default:
            return "User interface color theme option: Dark\nApplies the default dark visual theme to all windows and controls.";
    }
}

void ColorThemeRadioElement::click()
{
    auto preferences = gatedDatabase.get().getPreferences().getUIPreferences();
    preferences.setThemeIndex(targetTheme);
    UIPreferencesTab::apply(preferences);
    Polyglot::Node command = {
        {        "type", "UpdateUIPreferences" },
        {     "version",                     1 },
        { "preferences",  preferences.toNode() }
    };
    gatedDatabase.execute(command);
}

bool ColorThemeRadioElement::isSelected() const
{
    return gatedDatabase.get().getPreferences().getUIPreferences().getThemeIndex() == targetTheme;
}

int ColorThemeRadioElement::getTargetTheme() const
{
    return targetTheme;
}

std::string ColorThemeRadioElement::getThemeLabel(int theme)
{
    switch (theme)
    {
        case 1:
            return "Light";
        case 2:
            return "Classic";
        default:
            return "Dark";
    }
}

#ifdef ISTESTPROJECT
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ColorThemeRadioElement/ToggleAndSelection")
{
    GatedDatabase          db;
    UndoStack              us(db);
    Windows                w;
    ColorThemeRadioElement darkElem(0, db, us, w);
    ColorThemeRadioElement lightElem(1, db, us, w);

    REQUIRE(darkElem.getTargetTheme() == 0);
    REQUIRE(darkElem.getLabel() == "Dark");
    REQUIRE(darkElem.getType() == "Color Theme: Dark");
    REQUIRE(darkElem.isSelected());
    REQUIRE(!lightElem.isSelected());

    lightElem.click();
    REQUIRE(!darkElem.isSelected());
    REQUIRE(lightElem.isSelected());
    REQUIRE(db.get().getPreferences().getUIPreferences().getThemeIndex() == 1);
}
#endif
