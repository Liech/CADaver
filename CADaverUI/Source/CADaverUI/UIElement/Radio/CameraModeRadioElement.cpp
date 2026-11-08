#include "UIElement/Radio/CameraModeRadioElement.h"

#include "Database/Database.h"
#include "Database/Preferences/CameraPreferences.h"
#include "Database/Preferences/Preferences.h"
#include "Execution/GatedDatabase.h"

CameraModeRadioElement::CameraModeRadioElement(int targetMode, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows)
  : RadioUIElement("Camera Type", getModeLabel(targetMode), gatedDatabase, undoStack, windows)
  , targetMode(targetMode)
{
}

CameraModeRadioElement::~CameraModeRadioElement() {}

std::string CameraModeRadioElement::getDescription() const
{
    switch (targetMode)
    {
        case 1:
            return "Camera type option: Freecam\nFlies the camera freely using WASD and mouse look (cameratype).";
        default:
            return "Camera type option: Arcball\nRevolves the camera around a focal point or model (cameratype).";
    }
}

void CameraModeRadioElement::click()
{
    auto preferences = gatedDatabase.get().getPreferences().getCameraPreferences();
    preferences.requestModeChange(targetMode);
    Polyglot::Node command = {
        {        "type", "UpdateCameraPreferences" },
        {     "version",                         1 },
        { "preferences",      preferences.toNode() }
    };
    gatedDatabase.execute(command);
}

bool CameraModeRadioElement::isSelected() const
{
    return gatedDatabase.get().getPreferences().getCameraPreferences().getCameraMode() == targetMode;
}

int CameraModeRadioElement::getTargetMode() const
{
    return targetMode;
}

std::string CameraModeRadioElement::getModeLabel(int mode)
{
    switch (mode)
    {
        case 1:
            return "Freecam";
        default:
            return "Arcball";
    }
}

#ifdef ISTESTPROJECT
#include "Execution/UndoStack.h"
#include "Windows/Windows.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CameraModeRadioElement/ToggleAndSelection")
{
    GatedDatabase          db;
    UndoStack              us(db);
    Windows                w;
    CameraModeRadioElement arcballElem(0, db, us, w);
    CameraModeRadioElement freecamElem(1, db, us, w);

    REQUIRE(arcballElem.getTargetMode() == 0);
    REQUIRE(arcballElem.getLabel() == "Arcball");
    REQUIRE(arcballElem.getType() == "Camera Type: Arcball");
    REQUIRE(arcballElem.isSelected());
    REQUIRE(!freecamElem.isSelected());

    freecamElem.click();
    REQUIRE(!arcballElem.isSelected());
    REQUIRE(freecamElem.isSelected());
    REQUIRE(db.get().getPreferences().getCameraPreferences().getCameraMode() == 1);
}
#endif
