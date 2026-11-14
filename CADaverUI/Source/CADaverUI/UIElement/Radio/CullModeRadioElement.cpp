#include "UIElement/Radio/CullModeRadioElement.h"

#include "Database/Preferences/RenderPreferences.h"
#include "Execution/GatedDatabase.h"

CullModeRadioElement::CullModeRadioElement(int targetMode, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : RadioUIElement("Culling", getModeLabel(targetMode), gatedDatabase, undoStack, windows)
  , targetMode(targetMode)
{
}

CullModeRadioElement::~CullModeRadioElement() {}

std::string CullModeRadioElement::getDescription() const
{
    switch (targetMode)
    {
        case 1:
            return "Render option: Cull Front\nHides front-facing geometry surfaces during rendering.";
        case 2:
            return "Render option: Cull Back\nHides back-facing geometry surfaces during rendering.";
        default:
            return "Render option: Culling None\nDisables geometry face culling to render two-sided surfaces.";
    }
}

void CullModeRadioElement::click()
{
    Polyglot::Node    response = gatedDatabase.execute({
      {    "type", "GetRenderPreferences" },
      { "version",                      1 }
    });
    RenderPreferences preferences;
    if (response.contains("preferences"))
    {
        preferences.fromNode(response["preferences"]);
    }
    preferences.setCullMode(targetMode);
    Polyglot::Node command = {
        {        "type", "UpdateRenderPreferences" },
        {     "version",                         1 },
        { "preferences",      preferences.toNode() }
    };
    gatedDatabase.execute(command);
}

bool CullModeRadioElement::isSelected() const
{
    Polyglot::Node    response = gatedDatabase.execute({
      {    "type", "GetRenderPreferences" },
      { "version",                      1 }
    });
    RenderPreferences preferences;
    if (response.contains("preferences"))
    {
        preferences.fromNode(response["preferences"]);
    }
    return preferences.getCullMode() == targetMode;
}

int CullModeRadioElement::getTargetMode() const
{
    return targetMode;
}

std::string CullModeRadioElement::getModeLabel(int mode)
{
    switch (mode)
    {
        case 1:
            return "Front";
        case 2:
            return "Back";
        default:
            return "None";
    }
}

#ifdef ISTESTPROJECT
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CullModeRadioElement/ToggleAndSelection")
{
    GatedDatabase        db;
    UndoStack            us(db);
    Windows              w;
    CullModeRadioElement noneElem(0, db, us, w);
    CullModeRadioElement backElem(2, db, us, w);

    REQUIRE(noneElem.getTargetMode() == 0);
    REQUIRE(noneElem.getLabel() == "None");
    REQUIRE(noneElem.getType() == "Culling: None");
    REQUIRE(noneElem.isSelected());
    REQUIRE(!backElem.isSelected());

    backElem.click();
    REQUIRE(!noneElem.isSelected());
    REQUIRE(backElem.isSelected());
}
#endif
