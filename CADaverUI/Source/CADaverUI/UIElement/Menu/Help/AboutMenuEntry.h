#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for opening the application about dialog.
class AboutMenuEntry : public MenuEntry
{
  public:
    AboutMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~AboutMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
