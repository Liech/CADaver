#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for opening the commands info browser.
class CommandsInfoMenuEntry : public MenuEntry
{
  public:
    CommandsInfoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~CommandsInfoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
