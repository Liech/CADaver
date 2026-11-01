#pragma once

#include "UIElement/Menu/MenuEntry.h"

class ViewController;

// Menu leaf entry for rotating the camera 90 degrees left.
class ViewCubeRotateLeftMenuEntry : public MenuEntry
{
  public:
    ViewCubeRotateLeftMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ViewCubeRotateLeftMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    ViewController& viewController;
};
