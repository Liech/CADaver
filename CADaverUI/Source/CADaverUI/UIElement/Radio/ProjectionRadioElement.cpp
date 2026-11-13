#include "UIElement/Radio/ProjectionRadioElement.h"

#include "Database/Preferences/CameraPreferences.h"
#include "Execution/GatedDatabase.h"

ProjectionRadioElement::ProjectionRadioElement(bool orthographic, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : RadioUIElement("Projection", getProjectionLabel(orthographic), gatedDatabase, undoStack, windows)
  , orthographic(orthographic)
{
}

ProjectionRadioElement::~ProjectionRadioElement() {}

std::string ProjectionRadioElement::getDescription() const
{
    if (orthographic)
    {
        return "Camera projection option: Orthogonal\nRenders geometry with parallel projection and uniform scale (orthogonally).";
    }
    return "Camera projection option: Perspective\nRenders geometry with perspective depth and foreshortening.";
}

void ProjectionRadioElement::click()
{
    Polyglot::Node    response = gatedDatabase.execute({
      {    "type", "GetCameraPreferences" },
      { "version",                      1 }
    });
    CameraPreferences preferences;
    if (response.contains("preferences"))
    {
        preferences.fromNode(response["preferences"]);
    }
    preferences.setOrthographic(orthographic);
    Polyglot::Node command = {
        {        "type", "UpdateCameraPreferences" },
        {     "version",                         1 },
        { "preferences",      preferences.toNode() }
    };
    gatedDatabase.execute(command);
}

bool ProjectionRadioElement::isSelected() const
{
    Polyglot::Node    response = gatedDatabase.execute({
      {    "type", "GetCameraPreferences" },
      { "version",                      1 }
    });
    CameraPreferences preferences;
    if (response.contains("preferences"))
    {
        preferences.fromNode(response["preferences"]);
    }
    return preferences.isOrthographic() == orthographic;
}

bool ProjectionRadioElement::isOrthographic() const
{
    return orthographic;
}

std::string ProjectionRadioElement::getProjectionLabel(bool orthographic)
{
    if (orthographic)
    {
        return "Orthogonal";
    }
    return "Perspective";
}

#ifdef ISTESTPROJECT
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ProjectionRadioElement/ToggleAndSelection")
{
    GatedDatabase          db;
    UndoStack              us(db);
    Windows                w;
    ProjectionRadioElement perspElem(false, db, us, w);
    ProjectionRadioElement orthoElem(true, db, us, w);

    REQUIRE(!perspElem.isOrthographic());
    REQUIRE(perspElem.getLabel() == "Perspective");
    REQUIRE(perspElem.getType() == "Projection: Perspective");
    REQUIRE(perspElem.isSelected());
    REQUIRE(!orthoElem.isSelected());

    orthoElem.click();
    REQUIRE(!perspElem.isSelected());
    REQUIRE(orthoElem.isSelected());
}
#endif
