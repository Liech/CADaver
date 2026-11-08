#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for undoing the last command.
class UndoMenuEntry : public MenuEntry
{
  public:
    UndoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~UndoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual bool        render() override;
    virtual void        click() override;
};
