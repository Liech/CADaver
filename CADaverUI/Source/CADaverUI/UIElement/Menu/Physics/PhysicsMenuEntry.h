#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for opening the Physics Simulation window.
class PhysicsMenuEntry : public MenuEntry
{
  public:
    PhysicsMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~PhysicsMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
