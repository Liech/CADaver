#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for loading STEP CAD assets.
class LoadStepMenuEntry : public MenuEntry
{
  public:
    LoadStepMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~LoadStepMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
