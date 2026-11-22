#pragma once

#include "UIElement/Menu/MenuEntry.h"

// Menu entry under Create that opens the Lua function SVO generator window.
class FunctionSvoMenuEntry : public MenuEntry
{
  public:
    FunctionSvoMenuEntry(GatedDatabase& gatedDatabase, UndoStack& undoStack, Windows& windows);
    virtual ~FunctionSvoMenuEntry();

    virtual std::string getType() const override;
    virtual std::string getDescription() const override;
    virtual void        click() override;
};
