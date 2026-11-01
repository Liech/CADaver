#pragma once

#include "UIElement/MenuEntry.h"

// Menu leaf entry for creating procedural terrain.
class LandscapeMenuEntry : public MenuEntry
{
  public:
    LandscapeMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~LandscapeMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
