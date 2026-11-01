#pragma once

#include "UIElement/MenuEntry.h"

// Menu leaf entry for opening the windows and workspace overview dialog.
class WindowsInfoMenuEntry : public MenuEntry
{
  public:
    WindowsInfoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~WindowsInfoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
