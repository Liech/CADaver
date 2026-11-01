#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for redoing the previously reverted command.
class RedoMenuEntry : public MenuEntry
{
  public:
    RedoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~RedoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual bool        render() override;
    virtual void        click() override;
};
