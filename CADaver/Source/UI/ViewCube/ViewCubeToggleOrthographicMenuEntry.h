#pragma once

#include "UIElement/Menu/MenuEntry.h"

class ViewController;

// Menu leaf entry for toggling between perspective and orthographic camera projection.
class ViewCubeToggleOrthographicMenuEntry : public MenuEntry
{
  public:
    ViewCubeToggleOrthographicMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ViewCubeToggleOrthographicMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    ViewController& viewController;
};
