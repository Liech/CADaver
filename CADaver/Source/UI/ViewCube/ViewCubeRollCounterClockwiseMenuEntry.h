#pragma once

#include "UIElement/Menu/MenuEntry.h"

class ViewController;

// Menu leaf entry for rolling the camera 90 degrees counter-clockwise.
class ViewCubeRollCounterClockwiseMenuEntry : public MenuEntry
{
  public:
    ViewCubeRollCounterClockwiseMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ViewCubeRollCounterClockwiseMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    ViewController& viewController;
};
