#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for opening application preferences.
class PreferencesMenuEntry : public MenuEntry
{
  public:
    PreferencesMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~PreferencesMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
