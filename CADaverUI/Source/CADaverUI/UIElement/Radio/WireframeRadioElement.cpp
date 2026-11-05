#include "UIElement/Radio/WireframeRadioElement.h"

#include "Database/Database.h"
#include "Database/Preferences/Preferences.h"
#include "Database/Preferences/RenderPreferences.h"
#include "Execution/GatedDatabase.h"

WireframeRadioElement::WireframeRadioElement(int targetMode, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : RadioUIElement("Wireframe", getModeLabel(targetMode), gatedDatabase, undoStack, windows)
  , targetMode(targetMode)
{
}

WireframeRadioElement::~WireframeRadioElement() {}

std::string WireframeRadioElement::getDescription() const
{
    switch (targetMode)
    {
        case 1:
            return "Render option: Wireframe Overlay\nRenders wireframe edges on top of shaded geometry faces.";
        case 2:
            return "Render option: Wireframe Only\nRenders the wireframe edges of meshes without shading.";
        default:
            return "Render option: Wireframe Off\nDisables wireframe rendering to display shaded surfaces only.";
    }
}

void WireframeRadioElement::click()
{
    auto preferences = gatedDatabase.get().getPreferences().getRenderPreferences();
    preferences.setWireframeMode(targetMode);
    Polyglot::Node command = {
        {        "type", "UpdateRenderPreferences" },
        {     "version",                         1 },
        { "preferences",      preferences.toJson() }
    };
    gatedDatabase.execute(command);
}

bool WireframeRadioElement::isSelected() const
{
    return gatedDatabase.get().getPreferences().getRenderPreferences().getWireframeMode() == targetMode;
}

int WireframeRadioElement::getTargetMode() const
{
    return targetMode;
}

std::string WireframeRadioElement::getModeLabel(int mode)
{
    switch (mode)
    {
        case 1:
            return "Overlay";
        case 2:
            return "Only";
        default:
            return "Off";
    }
}

#ifdef ISTESTPROJECT
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("WireframeRadioElement/ToggleAndSelection")
{
    GatedDatabase         db;
    UndoStack             us(db);
    Windows               w;
    WireframeRadioElement offElem(0, db, us, w);
    WireframeRadioElement overlayElem(1, db, us, w);

    REQUIRE(offElem.getTargetMode() == 0);
    REQUIRE(offElem.getLabel() == "Off");
    REQUIRE(offElem.getType() == "Wireframe: Off");
    REQUIRE(offElem.isSelected());
    REQUIRE(!overlayElem.isSelected());

    overlayElem.click();
    REQUIRE(!offElem.isSelected());
    REQUIRE(overlayElem.isSelected());
    REQUIRE(db.get().getPreferences().getRenderPreferences().getWireframeMode() == 1);
}
#endif
