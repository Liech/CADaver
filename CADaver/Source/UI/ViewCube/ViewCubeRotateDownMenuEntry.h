#pragma once

#include "UIElement/Menu/MenuEntry.h"

class ViewController;

// Menu leaf entry for rotating the camera 90 degrees downward.
class ViewCubeRotateDownMenuEntry : public MenuEntry
{
  public:
    ViewCubeRotateDownMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ViewCubeRotateDownMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    ViewController& viewController;
};
