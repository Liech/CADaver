#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu leaf entry for quitting the application.
class QuitMenuEntry : public MenuEntry
{
  public:
    QuitMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows, bool& quitRequested);
    virtual ~QuitMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;

  private:
    bool& quitRequested;
};
