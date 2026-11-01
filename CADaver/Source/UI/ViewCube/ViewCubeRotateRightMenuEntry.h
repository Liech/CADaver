#pragma once

#include "UIElement/Menu/MenuEntry.h"

class ViewController;

// Menu leaf entry for rotating the camera 90 degrees right.
class ViewCubeRotateRightMenuEntry : public MenuEntry
{
  public:
    ViewCubeRotateRightMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ViewCubeRotateRightMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    ViewController& viewController;
};
