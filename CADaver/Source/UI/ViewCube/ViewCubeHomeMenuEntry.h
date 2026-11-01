#pragma once

#include "UIElement/Menu/MenuEntry.h"

class ViewController;

// Menu leaf entry for resetting camera view to fit the project geometry.
class ViewCubeHomeMenuEntry : public MenuEntry
{
  public:
    ViewCubeHomeMenuEntry(ViewController& viewController, GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~ViewCubeHomeMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    ViewController& viewController;
};
